cmake_minimum_required(VERSION 3.20)
project(MyEDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# WIN32 表示 GUI 子系统程序（入口 WinMain），否则按控制台程序链接会报“无法解析 main”
add_executable(MyEDA WIN32
    src/MyApp.cpp
    src/MyFrame.cpp
)

target_link_libraries(MyEDA PRIVATE ${wxWidgets_LIBRARIES})

# MSVC 默认按 GBK 解析无 BOM 的 UTF-8 源码，中文注释/字符串会编译报错，强制按 UTF-8 处理
if(MSVC)
    target_compile_options(MyEDA PRIVATE "/utf-8")
endif()

# 把 res 文件夹复制到可执行文件旁边
add_custom_command(TARGET MyEDA POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/res
    $<TARGET_FILE_DIR:MyEDA>/res
)
