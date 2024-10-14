#pragma once

// Template class for Vector
template <int n>
class Vector {
public:
    // Constructor
    Vector() {
        for (int i = 0; i < n; ++i)
            data_[i] = 0.0;
    }

    // Access to elements
    double& operator[](int i) {
        return data_[i];
    }

    const double& operator[](int i) const {
        return data_[i];
    }

    // Vector addition
    Vector<n> operator+(const Vector<n>& other) const {
        Vector<n> result;
        for (int i = 0; i < n; ++i) {
            result[i] = data_[i] + other[i];
        }
        return result;
    }

    // Vector subtraction
    Vector<n> operator-(const Vector<n>& other) const {
        Vector<n> result;
        for (int i = 0; i < n; ++i) {
            result[i] = data_[i] - other[i];
        }
        return result;
    }

private:
    double data_[n];
};

template <int n, int m>
class Matrix {
public:
    // Constructor
    Matrix()
    {
        memset(&(data_[0][0]), 0, sizeof(data_));
    }

    // Matrix addition
    Matrix<n, m> operator+(const Matrix<n, m>& other) const {
        Matrix<n, m> result;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                result.data_[i][j] = data_[i][j] + other.data_[i][j];
        return result;
    }

    // Matrix subtraction
    Matrix<n, m> operator-(const Matrix<n, m>& other) const {
        Matrix<n, m> result;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                result.data_[i][j] = data_[i][j] - other.data_[i][j];
        return result;
    }

    // Matrix multiplication
    template <int p>
    Matrix<n, p> operator*(const Matrix<m, p>& other) const {
        Matrix<n, p> result;
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < p; ++j)
                for (int k = 0; k < m; ++k)
                    result.data_[i][j] += data_[i][k] * other.data_[k][j];
        return result;
    }

    // Matrix-Vector multiplication
    Vector<n> operator*(const Vector<m>& vec) const {
        Vector<n> result;
        for (int i = 0; i < n; ++i) {
            result[i] = 0;
            for (int j = 0; j < m; ++j) {
                result[i] += data_[i][j] * vec[j];
            }
        }
        return result;
    }

    // Access to elements
    double& operator()(int i, int j) {
        return data_[i][j];
    }

    const double& operator()(int i, int j) const {
        return data_[i][j];
    }

    Matrix& zeros()
    {
        memset(data_, 0, sizeof(data_));
        return *this;
    }

    Matrix& eye()
    {
        memset(data_, 0, sizeof(data_));
        if (n > m)
        {
            for (int idx = 0; idx < m; idx++)
            {
                data_[idx][idx] = 1.0;
            }
        }
        else
        {
            for (int idx = 0; idx < n; idx++)
            {
                data_[idx][idx] = 1.0;
            }
        }
        return *this;
    }

    Matrix<n, m>  operator =(const Matrix<n, m>& other)
    {
        memcpy(&(data_[0][0]), &(other.data_[0][0]), sizeof(data_));
        return *this;
    }

private:
    double data_[n][m];
};

using Mat3x3 = Matrix<3, 3>;
using Mat4x4 = Matrix<4, 4>;
using Mat5x5 = Matrix<5, 5>;

using Vec3 = Vector<3>;
using Vec4 = Vector<4>;
using Vec5 = Vector<5>;