# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.4.1)

#Add OpenCV lib
include_directories(${OpenCV_DIR}/jni/include) #Path from gradle to OpenCV Cmake
add_library( lib_opencv SHARED IMPORTED )
set_target_properties(lib_opencv PROPERTIES IMPORTED_LOCATION ${OpenCV_DIR}/libs/${ANDROID_ABI}/libopencv_java4.so)

#Add Your Native Lib
add_library(native-lib SHARED native-lib.cpp)

#Add&Link Android Native Log lib with others libs
find_library(log-lib log)
target_link_libraries(native-lib lib_opencv ${log-lib})