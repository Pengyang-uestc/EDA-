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
    src/DrawingCanvas.cpp
)

target_link_libraries(MyEDA PRIVATE ${wxWidgets_LIBRARIES})

# 源码是 UTF-8(无BOM),MSVC 默认按 GBK 解析会报错;只用 /source-charset 解析源码,
# 字符串仍按系统编码(GBK)输出,wxString 才能正确显示中文。不能用 /utf-8(会导致界面中文乱码)
if(MSVC)
    target_compile_options(MyEDA PRIVATE "/source-charset:utf-8")
endif()

# 把 res 文件夹复制到可执行文件旁边
add_custom_command(TARGET MyEDA POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/res
    $<TARGET_FILE_DIR:MyEDA>/res
)

# 仿真引擎单元测试(不弹窗口):cmake --build build --target test_sim --config Debug
add_executable(test_sim EXCLUDE_FROM_ALL test/test_sim.cpp)
target_include_directories(test_sim PRIVATE src)
target_link_libraries(test_sim PRIVATE ${wxWidgets_LIBRARIES})
target_compile_options(test_sim PRIVATE "/source-charset:utf-8")
