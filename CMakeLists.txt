cmake_minimum_required(VERSION 3.30)
project(splash_screen)

set(CMAKE_CXX_STANDARD 23)

# 添加可执行文件
add_executable(splash_screen main.cpp)

# 添加包含目录
target_include_directories(splash_screen PUBLIC "third/glfw/include")

add_subdirectory(third/glfw)

# 链接 glfw3 库以及其他依赖库
target_link_libraries(splash_screen PUBLIC glfw)

# 解决 MSVCRT 库冲突
#target_link_libraries(splash_screen PRIVATE /NODEFAULTLIB:MSVCRT)
