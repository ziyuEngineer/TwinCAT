# 生成`.msgpack`文件
msgpack通过二进制格式，极大的提升了解析、加载指令的速度。

## 1. 传输的结构体类型：

|           | 数据类型 | 说明                    |
|------------|--------|-----------------------|
| `target`   | `double` | 目标值，设定的目标位置 / 速度 / 扭矩 |
| `additive_pos` | `double` | 位置补偿值       |
| `additive_vel` | `double` | 速度补偿值        |
| `additive_tor` | `double` | 力矩补偿值       |

## 2. 用 Python 生成 msgpack 文件

将上述 **NumPy 数组（`np.array<double, 4>`）** 转换并存储为 **MessagePack** 格式的示例。

### 2.1 安装依赖

确保已安装 `ormsgpack` 和 `numpy`：

```bash
pip install ormsgpack numpy
```
代码示例:
```bash
import ormsgpack
import numpy as np

array = np.arange(8100, dtype=np.float64).reshape(2025, 4)  # 生成形状为 (2025, 4) 的二维数组
with open("demo.msgpack", "wb") as f:
    f.write(ormsgpack.packb(array, option=ormsgpack.OPT_SERIALIZE_NUMPY))  # 序列化并写入demo.msgpack

```