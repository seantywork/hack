cmake_minimum_required(VERSION 3.5)
project(ecat_pkg)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
## Compile options for IgH libary and several Linux libraries (e.g lpthread)
add_compile_options(-g -w -Wall -Wextra -Wpedantic)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

## Defining paths and libraries to include in the next section.
set(etherlab_include ethercat/include)
set(etherlab_lib ethercat/lib)
set(emc_include ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(node_name "emc")

## Finding packages that'll be required for compilation.
## Don't forget to add packages if you use it in your code, otherwise you'll get build errors.

## Output executable name and requied cpp files for executable
file(GLOB emc_SRC
     "src/*.cc"
)

# Add include directories
include_directories(
  ${etherlab_include}
  ${emc_include}
)


link_directories(
  ${etherlab_lib}
)



add_executable(emc ${emc_SRC})



## Specifying libraries by using definitions above.
target_link_libraries(
  emc
  PRIVATE
  ethercat 
  Threads::Threads
  rt
)


## Don't forget to add dependencies to your build file, 
## Use find_package(x) then add dependecy for x. 

install(TARGETS emc
  DESTINATION build/${PROJECT_NAME})
