# RVBoards D1 CPU 使用率指示器

跑在 RVBoards D1 单板机上的 CPU 使用率指示器。读 /proc/stat 获取 CPU 使用率，写 /sys/class/leds/sunxi_led0[rg]/brightness 点亮红灯或绿灯，以此指示 CPU 使用率。实现的语言包括但不限于C、C++、Rust、Go、Java、Python。作为 RISC-V Linux 程序开发的 Demo 程序，比 Hello World 更好玩一点。

#### 程序功能
- 使用率 < 30%，点亮绿灯，比较亮。
- 30% < 使用率 < 60%，点亮绿灯，比较暗。
- 60% < 使用率 < 90%，点亮红灯，比较暗。
- 90% < 使用率，点亮红灯，比较亮。
- 每秒执行一次。

#### 程序对比
| 语言  | 程序大小 | 编译时间 | RSS | VM-Size |
|-----|----|------|---|---|
| C   |  3,366 + 676 + 8,288 = 12,330 |  2.5s    | 1.1MB | 1.4MB |
| C++ | 6,347 + 816 + 8,296 = 15,459|  22.4s    | 2.2MB | 3.7MB |
| Rust |  331,116 +  12,649 + 705 = 344,470| 9.4s  | 1.6MB | 68.0MB |
| Go | 1,421,367 + 90,480 + 194,232 = 1,706,079|  10.0s  | 1.8MB | 686.6MB |
| Java | - |  145.5s  | 23.0MB | 941.3MB |
| Python | - | - | 8.2MB | 10.3MB|
| MATLAB | - | - | 37.1MB | 110.2MB |

- 编译型语言使用默认的编译选项，未明确打开优化选项。
- 程序大小的4个值分别是 size 输出的 text 、data、bss、dec 字段。
- C、C++、Rust 程序是动态链接的，Go 程序不支持动态链接，是静态链接的。
- 编译时间，指的是在 RVBoards D1 上本地编译所需要的时间，不是交叉编译。Rust 的编译时间不包括依赖库的编译时间。

## 获取源代码

#### 安装 git
```
sudo apt-get install git
```

#### 克隆代码
```
git clone https://gitee.com/zoomdy/rvboards_d1_cpu_usage_indicator
```

## 编译/执行

#### C
系统自带编译器，不需要手动安装。

编译：
```
cd rvboards_d1_cpu_usage_indicator/c
make
```

执行：
```
sudo ./cpu_usage_indicator
```

#### C++
系统自带编译器，不需要手动安装。

编译：
```
cd rvboards_d1_cpu_usage_indicator/c++
make
```

执行：
```
sudo ./cpu_usage_indicator
```

#### Rust
安装 rust 和 cargo：
```
sudo apt-get install cargo
```

编译，会自动下载编译依赖库，如果下载很慢，那么参考 [Rust Crates 镜像使用帮助](https://lug.ustc.edu.cn/wiki/mirrors/help/rust-crates/) 使用国内镜像，`cargo -V` 查看 cargo 版本：
```
cd rvboards_d1_cpu_usage_indicator/rust
cargo build
```

执行：
```
sudo ./target/debug/cpu_usage_indicator
```

#### Go
安装 Go：
```
sudo apt-get install golang-go
```

编译：
```
cd rvboards_d1_cpu_usage_indicator/go
make
```

执行：
```
sudo ./cpu_usage_indicator
```

#### Java
安装 JDK：
```
sudo apt-get install default-jdk
```

编译，编译大约需要2分钟时间，耐心等待：
```
cd rvboards_d1_cpu_usage_indicator/java
make
```

执行：
```
sudo ./cpu_usage_indicator
```

#### Python
不记得预装系统是不是已经默认安装了，安装 Python3：
```
sudo apt-get install python3
```

执行：
```
sudo ./cpu_usage_indicator.py
```

#### NodeJS
目前（2021-06-03），Debain 软件库还未包括 nodejs，不能通过 apt-get 安装，需要手动安装，所幸 nodejs 移植团队有编译好的二进制包，下载页面：[https://github.com/v8-riscv/node/releases](https://github.com/v8-riscv/node/releases) 。下载 node-v14.8.0-linux-riscv64.tar.xz ，解压到 /opt 目录下。

执行 cpu_usage_indicator：

```
cd rvboards_d1_cpu_usage_indicator/nodejs
sudo /opt/node-v14.8.0-linux-riscv64/bin/node cpu_usage_indicator.js
```

#### MATLAB
安装 MATLAB 的开源替代 GNU Octave：
```
sudo apt-get install octave
```

执行：
```
sudo ./cpu_usage_indicator
```

