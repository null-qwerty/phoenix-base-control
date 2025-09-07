# Embedded System Framework

> V2 版本绝赞开发中，切换到 V2 分支查看最新进度。
>
> V2 版本将会是一个全新的框架，具有以下特点：
> - ~~降低对 CubeMX 的依赖，改用重新设计的接口进行管理；~~
> - ~~将 HAL 和 FreeRTOS 作为独立的子模块，便于更新和维护；~~ 
> - （CubeMX 还是太好用了，留给之后想 gank 掉的人移除吧 XD）
> - 使用 CRTP 模式替换原有动态多态接口，提升性能；
> - 将通信模块与其他模块进一步解耦；
> - 将编写代码生成脚本，支持 CubeMX 的配置导入。

用于 RoboMaster 高校系列赛的嵌入式系统框架，依赖 CubeMX 生成的代码，内置 dsp 快速计算。

## 代码结构

黄色部分为已有代码，也是核心部分。后续将逐步完善应用层代码。

![](docs/layer.svg)

## 使用方式

1. 将本仓库作为子模块添加到项目根目录；
2. 使用 CubeMX 生成代码，配置好 HAL，使用 cmake 工具链；
3. 将 `main.c` 中头文件剪切到 `main.h` 中，并将 `main.c` 改为 `main.cpp`；
4. 在 `CMakeLists.txt` 中启用 `C++` 语言，并添加以下内容：

```cmake
add_subdirectory(xxxxx) # 替换为子模块目录名
include_directories(
    # 添加其他需要的头文件目录
    ${FRAMEWORK_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} path/to/main.cpp) # 同时将 CubeMX 生成的 cmakelists 中添加 main.c 的一行删除

target_link_libraries(${PROJECT_NAME} 
    # 其他需要链接的库
    embedded_system_framework
)
```