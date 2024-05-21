cmake_minimum_required(VERSION 3.20)

project(N08_DB_HW04_Task01)

add_subdirectory("D:\\coding\\libpqxx-7.9.0" libpqxx-build)

add_executable(N08_DB_HW04_Task01 main.cpp)

target_compile_features(N08_DB_HW04_Task01 PRIVATE cxx_std_17) 

target_link_libraries(N08_DB_HW04_Task01 pqxx) 