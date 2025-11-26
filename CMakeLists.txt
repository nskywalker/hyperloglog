cmake_minimum_required(VERSION 3.31.6)
project(hyperloglog)

set(CMAKE_CXX_STANDARD 23)
add_compile_options(-fmodules-ts)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PUBLIC
    main.cpp
)

target_sources(${PROJECT_NAME}
        PUBLIC
        FILE_SET all_my_modules TYPE CXX_MODULES FILES
        hyperloglog.cppm
        src/murmurhash.cppm
)

target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
