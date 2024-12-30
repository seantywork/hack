cmake_minimum_required(VERSION 3.2)
project (cc_http_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (BOOST_DIR boost_1_81_0/build)

include_directories(
    include
    "${BOOST_DIR}/include"
    )


set ( PROJECT_LINK_LIBS libboost_system.a)

link_directories( 
    "${BOOST_DIR}/lib"
    )


file(GLOB server_source
    "src/*.cc"
)

add_executable(async_server.out 
    main_async.cc
    ${server_source}
    )


target_link_libraries(async_server.out
    ${PROJECT_LINK_LIBS}
)
