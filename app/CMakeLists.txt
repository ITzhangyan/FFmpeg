#指定需要cmake的最小版本
cmake_minimum_required(VERSION 3.10.2)
#file表示文件操作，使用相对路径，添加该路径下的所有以.cpp结尾的文件
file(GLOB SOURCE src/main/cpp/*.cpp)
#编译或者是添加一个动态库，名字是wanavpalyer，源文件是src/main/cpp/*.cpp
#SHARED 表示生成动态库或共享库  STATIC 表示生成静态库
add_library( # Sets the name of the library.
        wanavpalyer
        SHARED
        ${SOURCE})
#查找到指定的预编译库，并将它的路径存储在变量中。
#默认的搜索路径为 cmake 包含的系统库，因此如果是 NDK 的公共库只需要指定库的 name 即可。
#此处的log库是我们后面调试时需要用来打log日志的库
find_library( # Sets the name of the path variable.
        log-lib
        log)
#设置头文件搜索路径(和CMakeLists.txt同个路径的头文件无需设置)
include_directories(src/main/cpp/include)

#依赖的so库的存放路径 ，${CMAKE_SOURCE_DIR} 其实就是CMakeLists.txt文件所在文件夹
#${ANDROID_ABI}其实就是你编译的手机的cpu架构，比如armeabi、X86、mips64等等
set(my_lib_path  ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI})
#指定编译参数
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -L${my_lib_path}")
#关联我们自己的库和一些第三方库或者系统库
#把要链接的库的别名都写在这，如果是系统的库要用这个格式 ${库的名字}
target_link_libraries( # Specifies the target library.
        wanavpalyer
        avfilter avformat avcodec avutil swresample swscale
        ${log-lib}
        z
        OpenSLES
        android
        )