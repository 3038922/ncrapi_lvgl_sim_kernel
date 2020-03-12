#设置最低的编译版本
cmake_minimum_required(VERSION 3.10)
#项目名称
project(ncrapi_lvgl_sim_kernel)
#头文件路径
include_directories(
    ${TOOLCHAIN_INC_DIR} 
    "."
)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) #设置WIN库文件存放路径 
if(NOT MAKE_LVGL_DRIVE_STATIC_LIB)
    #枚举源文件
    file(GLOB_RECURSE SOURCES "lv_examples/*.c" "lv_examples/*.cpp"  "src/*.cpp" "src/*.c")
    #链接SDL2的静态库
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
        #寻找sdl2
        message(STATUS "linux  MAKE_LVGL_DRIVE_STATIC_LIB OFF")
        find_package(SDL2 REQUIRED SDL2)
        include_directories(${SDL2_INCLUDE_DIRS})
        # 指定生成 ncrapi_lvgl_sim_kernel 链接库
        add_library (ncrapi_lvgl_sim_kernel ${SOURCES})
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
        message(STATUS "windows MAKE_LVGL_DRIVE_STATIC_LIB OFF")
        #添加-D
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        # 指定生成 ncrapi_lvgl_sim_kernel 链接库
        add_library (ncrapi_lvgl_sim_kernel ${SOURCES})
    else()
        message(STATUS "无法判断属于哪种系统!")
    endif()
else() 
    #要建立的静态库
    file(GLOB_RECURSE LIBLVGL "lvgl/*.c")
    file(GLOB_RECURSE LIBDRIVERS "lv_drivers/*.c")
    #file(GLOB_RECURSE LIBRESOURCES "./src/system/logo.c" "./src/system/field.c")
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
        message(STATUS "linux  MAKE_LVGL_DRIVE_STATIC_LIB ON")
        #寻找sdl2
        find_package(SDL2 REQUIRED SDL2)
        include_directories(${SDL2_INCLUDE_DIRS})
        add_library(lvgl_gcc STATIC ${LIBLVGL})
        add_library(drivers_gcc STATIC ${LIBDRIVERS})
        #add_library(resources STATIC ${LIBRESOURCES}_linux)
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
        message(STATUS "windows  MAKE_LVGL_DRIVE_STATIC_LIB ON")
        #添加库文件
        add_library(lvgl_clang_win STATIC ${LIBLVGL})
        add_library(drivers_clang_win STATIC ${LIBDRIVERS})
        #add_library(resources STATIC ${LIBRESOURCES}_win)
    else()
        message(STATUS "无法判断属于哪种系统!")
    endif()
endif()








