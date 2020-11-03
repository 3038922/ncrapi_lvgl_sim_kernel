#设置最低的编译版本
cmake_minimum_required(VERSION 3.10)
#项目名称
project(ncrapi_lvgl_sim_kernel)
#设置c/c++标准
set(CMAKE_C_STANDARD 11)#C11
set(CMAKE_CXX_STANDARD 17)#C17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#头文件路径
include_directories(
    ${TOOLCHAIN_INC_DIR} 
    "./"
    "./freeRTOS/src"
    "../include"
)
#添加需要链接的库文件目录 新建好了 链接的时候用
LINK_DIRECTORIES(
"C:/Program Files (x86)/Windows Kits/10/Lib/10.0.18362.0/um/x64" 
"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.24.28314/lib/x64"
"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/MSVC/14.24.28314/atlmfc/lib/x64"
"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/VS/lib/x64"
"C:/Program Files (x86)/Windows Kits/10/lib/10.0.18362.0/ucrt/x64"
"C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/VS/UnitTest/lib"
"C:/Program Files (x86)/Windows Kits/NETFXSDK/4.7.2/lib/um/x64")
#关闭警告
add_compile_options(-Wno-unused-private-field)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib) #设置WIN库文件存放路径 

if(AS_A_SUBPROJECT)
    message(STATUS "ncrapi_lvgl_sim_kernel 作为子项目编译")
    if(NOT MAKE_LVGL_DRIVE_STATIC_LIB)
        #枚举源文件
        file(GLOB_RECURSE SOURCES "lv_examples/src/*.c" "lv_examples/src/*.cpp"  "src/*.cpp" "src/*.c")
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
        file(GLOB_RECURSE LIBLVGL "lvgl/src/*.c")
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
else()
    message(STATUS "ncrapi_lvgl_sim_kernel 作为独立项目编译")
    set (AS_A_SUBPROJECT 1)#设置成1 迫使后续模块使用静态库形式导入
    #######################加入freeRTOS模块#########################
    add_subdirectory ("./freeRTOS")
    set (EXTRA_LIBS ${EXTRA_LIBS} freeRTOS)
    ############################################################################
    #枚举本地源文件
    file(GLOB_RECURSE INCLUDES "lv_drivers/*.h" "lv_examples/*.h"  "lvgl/*.h"  "./*.h" )
    file(GLOB_RECURSE SOURCES "lv_drivers/*.c" "lv_examples/*.c"  "lvgl/src/*.c")
    #生成可执行文件
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
        message(STATUS "linux")
        #寻找sdl2
        find_package(SDL2 REQUIRED SDL2)
        include_directories(${SDL2_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE SDL2 lvgl_gcc drivers_gcc pthread ${EXTRA_LIBS})#不链接pthread easylog编译报错
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
        message(STATUS "windows")
        #添加-D
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
        LINK_DIRECTORIES("./lib")
        add_executable(main main.c ${SOURCES} ${INCLUDES})
        target_link_libraries(main PRIVATE SDL2 SDL2main  ${EXTRA_LIBS})#不链接pthread easylog编译报错
    else()
        message(STATUS "无法判断属于哪种系统!")
    endif()
    #添加一个run命令
    add_custom_target (run COMMAND ${EXECUTABLE_OUTPUT_PATH}/main)
endif()








