cmake_minimum_required(VERSION 3.20)
project(MyEDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

add_executable(MyEDA
    src/MyApp.cpp
    src/MyFrame.cpp
)

target_link_libraries(MyEDA PRIVATE ${wxWidgets_LIBRARIES})

# 把 res 文件夹复制到可执行文件旁边
add_custom_command(TARGET MyEDA POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/res
    $<TARGET_FILE_DIR:MyEDA>/res
)
