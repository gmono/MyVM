cmake_minimum_required(VERSION 3.20)
project(MyVMConsumer LANGUAGES CXX)

find_package(MyVM CONFIG REQUIRED)
add_executable(myvm-consumer ../../test/smoke.cpp)
target_link_libraries(myvm-consumer PRIVATE MyVM::vmlib)
