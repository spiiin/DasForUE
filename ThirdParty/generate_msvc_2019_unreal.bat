cd daScript

rd /Q /S cmake_temp
mkdir cmake_temp
cd cmake_temp

set CMAKE_GENERATOR=Visual Studio 16 2019

cmake -G "%CMAKE_GENERATOR%" -D CMAKE_CXX_FLAGS_RELEASE="/DDAS_NO_GLOBAL_NEW_AND_DELETE /DDAS_EXTERN_LOGERROR" -DCMAKE_INSTALL_PREFIX=. ..\
