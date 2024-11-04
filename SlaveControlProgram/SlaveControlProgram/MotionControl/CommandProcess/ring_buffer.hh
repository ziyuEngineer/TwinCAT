// Source: https://github.com/jnk0le/Ring-Buffer
#pragma once
#include <array>
#include <cstddef>
#include <functional>
#include <type_traits>

/*!
 * @brief Lock free, with no wasted slots ringbuffer implementation
 *
 * @param T Type of buffered elements
 * @param buffer_size Size of the buffer. Must be a power of 2.
 * @param fake_tso Omit generation of explicit barrier code to avoid unnecessary
 * instructions in tso scenario (e.g. simple microcontrollers/single core)
 * @param cacheline_size Size of the cache line (in bytes), to insert
 * appropriate padding in between indexes and buffer. Most modern CPU uses 64
 * bytes. Please set it according to the target machine. Setting a wrong cache
 * line size might lead to false sharing (updates to one-variable invalidate
 * caches for others), causing performance degradation.
 * @param index_t Type of array indexing type. Serves also as placeholder for
 * future implementations.
 */
template <typename T, size_t buffer_size, bool fake_tso = false,
    size_t cacheline_size = 0, typename index_t = size_t>
    class Ringbuffer {
    public:
        /*!
         * @brief Default constructor, will initialize head and tail indexes
         */
        Ringbuffer() : head_(0), tail_(0) {}

        /*!
         * @brief Special case constructor to premature out unnecessary initialization
         * code when object is instantiated in .bss section
         * @warning If object is instantiated on stack, heap or inside noinit section
         * then the contents have to be explicitly cleared before use
         * @param dummy Ignored
         */
        explicit Ringbuffer(int dummy) { (void)(dummy); }

        /*!
         * @brief Clear buffer from producer side
         * @warning function may return without performing any action if consumer
         * tries to read data at the same time
         */
        void producerClear() {
            // head modification will lead to underflow if cleared during consumer read
            // doing this properly with CAS is not possible without modifying the
            // consumer code
            consumerClear();
        }

        /*!
         * @brief Clear buffer from consumer side
         */
        void consumerClear() { tail_ = head_; }

        /*!
         * @brief Check if buffer is empty
         * @return True if buffer is empty
         */
        [[nodiscard]] bool isEmpty() const { return readAvailable() == 0; }

        /*!
         * @brief Check if buffer is full
         * @return True if buffer is full
         */
        [[nodiscard]] bool isFull() const { return writeAvailable() == 0; }

        /*!
         * @brief Check how many elements can be read from the buffer
         * @return Number of elements that can be read
         */
        [[nodiscard]] index_t readAvailable() const { return head_ - tail_; }

        /*!
         * @brief Check how many elements can be written into the buffer
         * @return Number of free slots that can be be written
         */
        [[nodiscard]] index_t writeAvailable() const {
            return buffer_size - (head_ - tail_);
        }

        /*!
         * @brief Inserts data into internal buffer, without blocking
         * @param data element to be inserted into internal buffer
         * @return True if data was inserted
         */
        bool insert(T data) {
            index_t tmp_head = head_;

            if ((tmp_head - tail_) == buffer_size) {
                return false;
            }
            data_buff_[tmp_head++ & kBufferMask] = data;

            head_ = tmp_head;

            return true;
        }

        /*!
         * @brief Inserts data into internal buffer, without blocking; data is passed
         * by reference
         * @param data
         * @return
         */
        bool insertPassByReference(const T& data) {
            index_t tmp_head = head_;

            if ((tmp_head - tail_) == buffer_size) {
                return false;
            }
            data_buff_[tmp_head++ & kBufferMask] = data;

            head_ = tmp_head;

            return true;
        }

        /*!
         * @brief Inserts data into internal buffer, without blocking
         * @param[in] data Pointer to memory location where element, to be inserted
         * into internal buffer, is located
         * @return True if data was inserted
         */
        bool insert(const T* data) {
            index_t tmp_head = head_;

            if ((tmp_head - tail_) == buffer_size) {
                return false;
            }
            else {
                data_buff_[tmp_head++ & kBufferMask] = *data;

                head_ = tmp_head;
            }
            return true;
        }

        /*!
         * @brief Inserts data returned by callback function, into internal buffer,
         * without blocking
         *
         * This is a special purpose function that can be used to avoid redundant
         * availability checks in case when acquiring data have a side effects (like
         * clearing status flags by reading a peripheral data register)
         *
         * @param get_data_callback Pointer to callback function that returns element
         * to be inserted into buffer ͨ
         * @return True if data was inserted and callback called
         */
        bool insertFromCallbackWhenAvailable(std::function<T()> get_data_callback) {
            index_t tmp_head = head_;

            if ((tmp_head - tail_) == buffer_size) {
                return false;
            }
            else {
                // execute callback only when there is space in buffer
                data_buff_[tmp_head++ & kBufferMask] = get_data_callback();

                head_ = tmp_head;
            }
            return true;
        }

        /*!
         * @brief Removes single element without reading
         * @return True if one element was removed
         */
        bool remove() {
            index_t tmp_tail = tail_;

            if (tmp_tail == head_) {
                return false;
            }
            tail_ = ++tmp_tail; // release in case data was loaded/used before
            return true;
        }

        /*!
         * @brief Removes multiple elements without reading and storing it elsewhere
         * @param cnt Maximum number of elements to remove
         * @return Number of removed elements
         */
        size_t remove(size_t cnt) {
            index_t tmp_tail = tail_;
            index_t avail = head_ - tmp_tail;

            cnt = (cnt > avail) ? avail : cnt;

            tail_ = tmp_tail + cnt;
            return cnt;
        }

        /*!
         * @brief Reads one element from internal buffer without blocking
         * @param[out] data Reference to memory location where removed element will be
         * stored
         * @return True if data was fetched from the internal buffer
         */
        bool remove(T& data) {
            return remove(&data); // references are anyway implemented as pointers
        }

        /*!
         * @brief Reads one element from internal buffer without blocking
         * @param[out] data Pointer to memory location where removed element will be
         * stored
         * @return True if data was fetched from the internal buffer
         * �ȸ�ֵ���Ƴ�
         */
        bool remove(T* data) {
            index_t tmp_tail = tail_;

            if (tmp_tail == head_) {
                return false;
            }
            else {
                *data = data_buff_[tmp_tail++ & kBufferMask];

                tail_ = tmp_tail;
            }
            return true;
        }

        /*!
         * @brief Gets the first element in the buffer on consumed side
         *
         * It is safe to use and modify item contents only on consumer side
         *
         * @return Pointer to first element, nullptr if buffer was empty
         */
        T* peek() {
            index_t tmp_tail = tail_;

            if (tmp_tail == head_) {
                return nullptr;
            }
            else {
                return &data_buff_[tmp_tail & kBufferMask];
            }
        }

        /*!
         * @brief Gets the n'th element on consumed side
         *
         * It is safe to use and modify item contents only on consumer side
         *
         * @param index Item offset starting on the consumed side
         * @return Pointer to requested element, nullptr if index exceeds storage
         */
        T* at(size_t index) {
            index_t tmp_tail = tail_;

            if ((head_ - tmp_tail) <= index) {
                return nullptr;
            }
            else {
                return &data_buff_[(tmp_tail + index) & kBufferMask];
            }
        }

        /*!
         * @brief Gets the n'th element on consumed side
         *
         * Unchecked operation, assumes that software already knows if the element can
         * be used, if requested index is out of bounds then reference will point to
         * somewhere inside the buffer The isEmpty() and readAvailable() will place
         * appropriate memory barriers if used as loop limiter It is safe to use and
         * modify T contents only on consumer side
         *
         * @param index Item offset starting on the consumed side
         * @return Reference to requested element, undefined if index exceeds storage
         * count
         */
        T& operator[](size_t index) {
            return data_buff_[(tail_ + index) & kBufferMask];
        }

        /*!
         * @brief Insert multiple elements into internal buffer without blocking
         *
         * This function will insert as much data as possible from given buffer.
         *
         * @param[in] buff Pointer to buffer with data to be inserted from
         * @param count Number of elements to write from the given buffer
         * @return Number of elements written into internal buffer
         */
        size_t writeBuff(const T* buff, size_t count);

        /*!
         * @brief Insert multiple elements into internal buffer without blocking
         *
         * This function will continue writing new entries until all data is written
         * or there is no more space. The callback function can be used to indicate to
         * consumer that it can start fetching data.
         *
         * @warning This function is not deterministic
         *
         * @param[in] buff Pointer to buffer with data to be inserted from
         * @param count Number of elements to write from the given buffer
         * @param count_to_callback Number of elements to write before calling a
         * callback function in first loop
         * @param execute_data_callback Pointer to callback function executed after
         * every loop iteration
         * @return Number of elements written into internal  buffer
         */
        size_t writeBuff(const T* buff, size_t count, size_t count_to_callback,
            std::function<void()> callback_function);

        /*!
         * @brief Load multiple elements from internal buffer without blocking
         *
         * This function will read up to specified amount of data.
         *
         * @param[out] buff Pointer to buffer where data will be loaded into
         * @param count Number of elements to load into the given buffer
         * @return Number of elements that were read from internal buffer
         */
        size_t readBuff(T* buff, size_t count);

        /*!
         * @brief Load multiple elements from internal buffer without blocking
         *
         * This function will read up to specified amount of data or until a callback
         * function returns true. The data causing the callback to return true will be
         * loaded.
         *
         * @param[out] buff Pointer to buffer where data will be loaded into
         * @param count Number of elements to load into the given buffer
         * @param callback Pointer to callback function that will be executed after
         * every read operation
         * @param is_triggered Reference to boolean variable that will be set to true
         * if callback returned true
         * @return Number of elements that were read from internal buffer
         */
        size_t readBuffUntil(T* buff, size_t count,
            std::function<bool(const T&)> callback,
            bool& is_triggered);

        /*!
         * @brief Load multiple elements from internal buffer without blocking
         *
         * This function will read up to specified amount of data or until a callback
         * function returns true. The data causing the callback to return true will
         * **NOT** be loaded.
         *
         * @param[out] buff Pointer to buffer where data will be loaded into
         * @param count Number of elements to load into the given buffer
         * @param callback Pointer to callback function that will be executed after
         * every read operation
         * @param is_triggered Reference to boolean variable that will be set to true
         * if callback returned true
         * @return Number of elements that were read from internal buffer
         */
        size_t readBuffBefore(T* buff, size_t count,
            std::function<bool(const T&)> callback,
            bool& is_triggered);

        /*!
         * @brief Load multiple elements from internal buffer without blocking
         *
         * This function will continue reading new entries until all requested data is
         * read or there is nothing more to read. The callback function can be used to
         * indicate to producer that it can start writing new data.
         *
         * @warning This function is not deterministic
         *
         * @param[out] buff Pointer to buffer where data will be loaded into
         * @param count Number of elements to load into the given buffer
         * @param count_to_callback Number of elements to load before calling a
         * callback function in first iteration
         * @param execute_data_callback Pointer to callback function executed after
         * every loop iteration
         * @return Number of elements that were read from internal buffer
         */
        size_t readBuff(T* buff, size_t count, size_t count_to_callback,
            std::function<void()> callback_function);
        index_t getHead() const { return head_; }
        index_t getBufferSize() const { return buffer_size; }
        index_t getTail() const { return tail_; }

    private:
        constexpr static index_t kBufferMask =
            buffer_size - 1; //!< bitwise mask for a given buffer size

        alignas(cacheline_size) index_t head_; //!< head index
        alignas(cacheline_size) index_t tail_; //!< tail index

        // put buffer after variables so everything can be reached with short offsets
        alignas(
            cacheline_size) std::array<T, buffer_size> data_buff_; //!< actual buffer

        // let's assert that no UB will be compiled in
        static_assert((buffer_size != 0), "buffer cannot be of zero size");
        static_assert((buffer_size& kBufferMask) == 0,
            "buffer size is not a power of 2");
        static_assert(sizeof(index_t) <= sizeof(size_t),
            "indexing type size is larger than size_t, operation is not "
            "lock free and doesn't make sense");
};

template <typename T, size_t buffer_size, bool fake_tso, size_t cacheline_size,
    typename index_t>
    size_t Ringbuffer<T, buffer_size, fake_tso, cacheline_size, index_t>::writeBuff(
        const T* buff, size_t count) {
    index_t available = 0;
    index_t tmp_head = head_;
    size_t to_write = count;

    available = buffer_size - (tmp_head - tail_);

    for (size_t i = 0; i < to_write; i++) {
        data_buff_[tmp_head++ & kBufferMask] = buff[i];
    }

    head_ = tmp_head;

    return to_write;
}

template <typename T, size_t buffer_size, bool fake_tso, size_t cacheline_size,
    typename index_t>
    size_t Ringbuffer<T, buffer_size, fake_tso, cacheline_size, index_t>::writeBuff(
        const T* buff, size_t count, size_t count_to_callback,
        // NOLINTNEXTLINE(performance-unnecessary-value-param)
        std::function<void()> callback_function) {
    size_t written = 0;
    index_t available = 0;
    index_t tmp_head = head_;
    size_t to_write = count;

    if (count_to_callback != 0 && count_to_callback < count) {
        to_write = count_to_callback;
    }

    while (written < count) {
        available = buffer_size - (tmp_head - tail_);

        if (available == 0) {
            // less than ??
            break;
        }

        if (to_write > available) {
            // do not write more than we can
            to_write = available;
        }

        while ((to_write--) > 0U) {
            data_buff_[tmp_head++ & kBufferMask] = buff[written++];
        }

        head_ = tmp_head;

        if (callback_function) {
            callback_function();
        }

        to_write = count - written;
    }

    return written;
}

template <typename T, size_t buffer_size, bool fake_tso, size_t cacheline_size,
    typename index_t>
    size_t Ringbuffer<T, buffer_size, fake_tso, cacheline_size, index_t>::readBuff(
        T* buff, size_t count) {
    index_t available = 0;
    index_t tmp_tail = tail_;
    size_t to_read = count;

    available = head_ - tmp_tail;

    if (available < count) {
        // do not read more than we can
        to_read = available;
    }

    // maybe divide it into 2 separate reads
    for (size_t i = 0; i < to_read; i++) {
        buff[i] = data_buff_[tmp_tail++ & kBufferMask];
    }

    tail_ = tmp_tail;

    return to_read;
}

template <typename T, size_t buffer_size, bool fake_tso, size_t cacheline_size,
    typename index_t>
    size_t
    Ringbuffer<T, buffer_size, fake_tso, cacheline_size, index_t>::readBuffUntil(
        T* buff, size_t count, std::function<bool(const T&)> callback,
        bool& is_triggered) {
    index_t available = 0;
    index_t tmp_tail = tail_;
    size_t to_read = count;

    available = head_ - tmp_tail;

    if (available < count) {
        // do not read more than we can
        to_read = available;
    }

    // maybe divide it into 2 separate reads
    for (size_t i = 0; i < to_read; i++) {
        buff[i] = data_buff_[tmp_tail++ & kBufferMask];
        if (callback(buff[i])) {
            is_triggered = true;
            to_read = i + 1;
            break;
        }
    }

    tail_ = tmp_tail;

    return to_read;
}

template <typename T, size_t buffer_size, bool fake_tso, size_t cacheline_size,
    typename index_t>
    size_t
    Ringbuffer<T, buffer_size, fake_tso, cacheline_size, index_t>::readBuffBefore(
        T* buff, size_t count, std::function<bool(const T&)> callback,
        bool& is_triggered) {
    index_t available = 0;
    index_t tmp_tail = tail_;
    size_t to_read = count;

    available = head_ - tmp_tail;

    if (available < count) {
        // do not read more than we can
        to_read = available;
    }

    // maybe divide it into 2 separate reads
    for (size_t i = 0; i < to_read; i++) {
        buff[i] = data_buff_[tmp_tail & kBufferMask];
        if (callback(buff[i])) {
            is_triggered = true;
            to_read = i;
            break;
        }
        tmp_tail++;
    }

    tail_ = tmp_tail;

    return to_read;
}

template <typename T, size_t buffer_size, bool fake_tso, size_t cacheline_size,
    typename index_t>
    size_t Ringbuffer<T, buffer_size, fake_tso, cacheline_size, index_t>::readBuff(
        T* buff, size_t count, size_t count_to_callback,
        // NOLINTNEXTLINE(performance-unnecessary-value-param)
        std::function<void()> callback_function) {
    size_t read = 0;
    index_t available = 0;
    index_t tmp_tail = tail_;
    size_t to_read = count;

    if (count_to_callback != 0 && count_to_callback < count) {
        to_read = count_to_callback;
    }

    while (read < count) {
        available = head_ - tmp_tail;

        if (available == 0) {
            // less than ??
            break;
        }

        if (to_read > available) {
            // do not write more than we can
            to_read = available;
        }

        while ((to_read--) != 0U) {
            buff[read++] = data_buff_[tmp_tail++ & kBufferMask];
        }

        tail_.stor = tmp_tail;

        if (callback_function) {
            callback_function();
        }

        to_read = count - read;
    }

    return read;
}
