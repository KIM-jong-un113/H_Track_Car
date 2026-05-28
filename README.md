# H_Track_Car — 2024 电赛 H 题（自动行驶小车）

基于 **TI MSPM0G3507** + 铁头山羊分层架构的巡线小车项目。

## 硬件清单

| 模块 | 型号/说明 |
|------|----------|
| 主控 | MSPM0G3507 (Cortex-M0+, 80MHz) |
| 陀螺仪 | MPU6050 六轴 (I2C0, 400kHz) |
| 电机驱动 | TB6612 双 H 桥 |
| 编码器 | 霍尔编码器 ×2 (4 倍频解码) |
| 灰度传感器 | 8 路 (TCRT5000) |
| 电源 | 电池 + 降压模块 |

## 项目结构

```
H_Track_Car/
├── my_lib/                 # 底层库（纯算法，不依赖具体 MCU）
│   ├── pid.h / pid.c       # PID 控制器（积分限幅 + 输出限幅 + LPF 级联）
│   ├── lpf.h / lpf.c       # 一阶低通滤波器
│   ├── qmath.h / qmath.c   # 快速三角函数（1024 点查表）
│   ├── task.h              # 协作式任务调度宏 PERIODIC()
│   ├── delay.h / delay.c   # 延时函数 + GetTick()/GetUs() 时间戳
│   └── sys.h / sys.c       # GPIO 宏、PWM 设置、printf 重定向、限幅
│
├── user/                   # 应用层（一个 .c 对应一个硬件模块）
│   ├── app_gyro.h / .c     # MPU6050 陀螺仪驱动 (I2C)
│   ├── app_encoder.h / .c  # 霍尔编码器驱动
│   ├── app_motor.h / .c    # TB6612 电机驱动
│   ├── app_sensor.h / .c   # 8 路灰度传感器 + 巡线算法
│   ├── app_control.h / .c  # 核心控制（双环 PID + 状态机）
│   ├── app_bat.h / .c      # 电池电压检测
│   └── main.c              # 主程序入口
│
├── test/                   # 各模块独立测试
│   ├── test_gyro.c         # 测试陀螺仪
│   ├── test_encoder.c      # 测试编码器
│   ├── test_motor.c        # 测试电机
│   └── test_sensor.c       # 测试灰度传感器
│
├── Debug/                  # 编译中间文件 + 输出
├── main.syscfg             # SysConfig 硬件配置
├── build.ps1               # 编译脚本
└── README.md
```

## 分层原则

| 层 | 可 include | 规则 |
|----|-----------|------|
| `my_lib/` | 标准库 + MSPM0 DriverLib | 不依赖 `user/` |
| `user/` | `my_lib/` + DriverLib | 一个 .c 对应一个硬件模块 |
| `test/` | `my_lib/` + `user/` | 每个 test 独立可编译（需修改 build.ps1） |

## 环境要求

- **编译器**: TI ARM Clang 4.0.4.LTS (`tiarmclang.exe`)
- **SDK**: MSPM0 SDK 2.10.00.04
- **IDE** (可选): CCS Theia 20.5.1

编译器路径和 SDK 路径在 `build.ps1` 中配置：

```powershell
$CompilerDir = "E:\Study\DianSai\TI\ccs\tools\compiler\ti-cgt-armllvm_4.0.4.LTS"
$SDKRoot     = "E:\Study\DianSai\TI\mspm0_sdk_2_10_00_04"
```

如果路径不同，需要修改 `build.ps1` 中这两个变量。

## 编译

在项目根目录运行：

```powershell
.\build.ps1
```

输出文件：`Debug\H_Track_Car.out`

## 烧录

使用 CCS Theia 或 Uniflash 将 `H_Track_Car.out` 烧录到 MSPM0G3507。

## 按键操作

| 按键 | 功能 |
|------|------|
| **S2** (PB1) | 切换模式：0→1→2→3→4→0（短按循环） |
| **EN** (PB4) | 使能/失能小车控制 |

- 上电后默认为 mode=0（停车）、flag_en=0（未使能）
- 按 **EN** 使能后小车开始运行

## 模式说明

| mode | 含义 |
|------|------|
| 0 | 停车 |
| 1 | 直线巡线（AB 段往返） |
| 2 | 绕桩巡线（A→B→C→D→A 完整一圈） |
| 3 | 复杂赛道（指定路线巡线一次） |
| 4 | 复杂赛道（循环三圈后停止） |

## 模块测试

每个 test 文件是**独立可编译的最小程序**，只初始化被测模块，printf 输出测试数据到串口。

### test_gyro.c — 陀螺仪测试

**测试内容**：MPU6050 初始化 + 读取 Yaw/Pitch/Roll 欧拉角。

**输出**（串口 115200bps）：
```
Yaw: 0.5  Pitch: -2.3  Roll: 1.1
Yaw: 0.6  Pitch: -2.4  Roll: 1.1
```

**操作**：旋转小车观察 Yaw 变化，俯仰/横滚小车观察 Pitch/Roll 变化。

**编译方法**：修改 `build.ps1` 中 `$UserSources` 为：
```powershell
$UserSources = @("$ProjectRoot\test\test_gyro.c", ...my_lib 源文件...)
```

或直接用 CCS 新建项目，将 `test/test_gyro.c` 作为 main.c 编译。

### test_encoder.c — 编码器测试

**测试内容**：初始化编码器，读取左右轮速度脉冲数。

**输出**（串口 115200bps）：
```
EncoderA: 15  EncoderB: 14
EncoderA: 16  EncoderB: 15
```

**操作**：用手转动车轮，观察脉冲计数变化。正转为正数，反转为负数。

**判断标准**：匀速转动时读数稳定，不跳变、不丢数即为正常。

### test_motor.c — 电机测试

**测试内容**：固定 PWM 正转 2s → 停 1s → 反转 2s → 停止。

**操作**：
1. 将小车架起（车轮悬空）
2. 烧录并运行
3. 观察：左右轮先正转 2 秒，停 1 秒，再反转 2 秒

**判断标准**：正转时两轮方向一致（向前），反转时两轮方向一致（向后）。如某轮方向反了，对调该路电机的 AIN1/AIN2 接线或修改 `Set_Pwm()` 中的方向逻辑。

### test_sensor.c — 灰度传感器测试

**测试内容**：读取 8 路灰度值 + 加权巡线偏差。

**输出**（串口 115200bps）：
```
P1:1 P2:1 P3:0 P4:0 P5:0 P6:0 P7:1 P8:1  bias:7
P1:1 P2:1 P3:1 P4:0 P5:0 P6:0 P7:1 P8:1  bias:12
```

**操作**：将传感器放在黑线上来回移动，观察 bias 变化。

**判断标准**：
- 在黑线上：对应探头读到 0（低电平），`Px=0`
- 在白色区域：对应探头读到 1（高电平），`Px=1`
- `bias` 为正 → 黑线偏右，小车应左转
- `bias` 为负 → 黑线偏左，小车应右转

> 注意：灰度传感器的电平逻辑取决于模块设计。如果黑白电平反过来，需要在 `app_sensor.h` 中给 P1~P8 宏加 `!` 取反。

## SysConfig 引脚分配

| 外设 | 引脚 | 功能 |
|------|------|------|
| PWM_0 CH0 | PB8 | 左电机 PWM |
| PWM_0 CH1 | PB12 | 右电机 PWM |
| DIR_AIN1 | PB10 | 左电机方向 |
| DIR_AIN2 | PB23 | 左电机方向 |
| DIR_BIN1 | PB26 | 右电机方向 |
| DIR_BIN2 | PB27 | 右电机方向 |
| EncoderA | PA31, PA12 | 左编码器 (4×) |
| EncoderB | PB24, PB9 | 右编码器 (4×) |
| UART0 TX/RX | PA10, PA11 | printf 调试串口 |
| I2C1 SCL/SDA | PB2, PB3 | MPU6050 陀螺仪 |
| Sensor P1-P8 | PA1,PA7,PA14,PB5,PB11,PB0,PB6,PB7 | 灰度传感器 |
| KEY S2 | PB1 | 模式切换按键 |
| KEY EN | PB4 | 使能按键 |
| LED | PA13 | 状态指示灯 |

## PID 参数

| 环路 | Kp | Ki | Kd | 输出限幅 |
|------|----|----|----|---------|
| 左轮速度环 | 40 | 0.13 | 0.1 | ±3800 |
| 右轮速度环 | 40 | 0.13 | 0.1 | ±3800 |
| 偏航角环 | 1 | 0 | 0 | ±40 |

基础速度 `Speed_Middle = 30`（在 [app_control.c](user/app_control.c) 中修改）。

## 参考项目

- 原竞赛项目：`E:\Study\DianSai\2024-Electronic-Design-Competition-master\`
- 铁头山羊平衡车教程：`E:\Study\TieTouShanYang\PinHengChe_YuanMa\`
