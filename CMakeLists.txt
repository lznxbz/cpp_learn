cmake_minimum_required(VERSION 3.20)
project(MyStudyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动把当前目录下所有 .cpp 文件识别为独立的可执行程序
file(GLOB files *.cpp)
foreach(file ${files})
    get_filename_component(name ${file} NAME_WE)
    add_executable(${name} ${file})
endforeach()
