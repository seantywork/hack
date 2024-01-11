cmake_minimum_required(VERSION 3.5)
project(roscancomm)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()


if(ROSCANCOMM_DEBUG)
  add_compile_options(-g)
endif()

add_compile_options(-Wall -Wno-unused-parameter -DCO_SINGLE_THREAD -g )

find_package(Threads REQUIRED)

if(STANDALONE_BUILD)

  set (CANCOMM .)

  set (DRV_SRC CANopenLinux)

  set (CANOPEN_SRC CANopenLinux/CANopenNode)

  set (APPL_SRC CANopenLinux/CANopenNode/example)

else()

  set (CANCOMM hardware/roscancomm)

  set (DRV_SRC hardware/roscancomm/CANopenLinux)

  set (CANOPEN_SRC hardware/roscancomm/CANopenLinux/CANopenNode)

  set (APPL_SRC hardware/roscancomm/CANopenLinux/CANopenNode/example)

endif()

file(
  GLOB roscancomm_cancomm_src
  "${CANCOMM}/cancomm.c"
)


file(
  GLOB roscancomm_drv_src
  "${DRV_SRC}/CO_driver.c"
  "${DRV_SRC}/CO_epoll_interface.c"
  "${DRV_SRC}/CO_error.c"
  "${DRV_SRC}/CO_storageLinux.c"

)

file(
  GLOB roscancomm_canopen_src1
  "${CANOPEN_SRC}/301/*.c"
)

file(
  GLOB roscancomm_canopen_src3
  "${CANOPEN_SRC}/303/*.c"
)

file(
  GLOB roscancomm_canopen_src4
  "${CANOPEN_SRC}/304/*.c"
)

file(
  GLOB roscancomm_canopen_src5
  "${CANOPEN_SRC}/305/*.c"
)

file(
  GLOB roscancomm_canopen_src9
  "${CANOPEN_SRC}/309/*.c"
)

file(
  GLOB roscancomm_canopen_srcstorage
  "${CANOPEN_SRC}/storage/CO_storage.c"
)

file(
  GLOB roscancomm_canopen_srcextra
  "${CANOPEN_SRC}/extra/CO_trace.c"
)

file(
  GLOB roscancomm_canopen_srcroot
  "${CANOPEN_SRC}/CANopen.c"
)


file(
  GLOB roscancomm_appl_src
  "${APPL_SRC}/OD.c"
)

file(
  GLOB roscancomm_cancomm_header
  "${CANCOMM}/*.h"
)


file(
  GLOB roscancomm_drv_header
  "${DRV_SRC}/*.h"
)

file(
  GLOB roscancomm_canopen_header
  "${CANOPEN_SRC}/*.h"
)

file(
  GLOB roscancomm_appl_header
  "${APPL_SRC}/*.h"
)


set(
  ROSCC_SRC 
  ${roscancomm_cancomm_src}
  ${roscancomm_drv_src} 
  ${roscancomm_canopen_src1}
  ${roscancomm_canopen_src3}
  ${roscancomm_canopen_src4}
  ${roscancomm_canopen_src5}
  ${roscancomm_canopen_src9}
  ${roscancomm_canopen_srcstorage}
  ${roscancomm_canopen_srcextra}
  ${roscancomm_canopen_srcroot}
  ${roscancomm_appl_src}
  )

set(
  ROSCC_HEADER 
  ${roscancomm_cancomm_header}
  ${roscancomm_drv_header} 
  ${roscancomm_canopen_header} 
  ${roscancomm_appl_header}
  )


if(ROSCANCOMM_ELF)
  add_executable(
    roscancomm
    "${ROSCC_SRC}"
  )
else()
  add_library(
    roscancomm
    SHARED
    "${ROSCC_SRC}"
  )

  target_link_libraries(
    roscancomm
    PUBLIC
    Threads::Threads
  )
endif()



target_include_directories(
  roscancomm
  PRIVATE
  "${CANCOMM}"
  "${DRV_SRC}"
  "${CANOPEN_SRC}"
  "${APPL_SRC}"
)

install(
  FILES ${ROSCC_HEADER}
  DESTINATION include/roscancomm
)

if(ROSCANCOMM_ELF)
  install(
    TARGETS roscancomm
    RUNTIME DESTINATION /usr/bin
  )
else()
  install(
    TARGETS roscancomm
    EXPORT roscancomm
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
  )
endif()

if(NOT DEFINED STANDALONE_BUILD)
  install(
    EXPORT roscancomm
    DESTINATION lib
    FILE roscancomm-Config.cmake
    )
endif()
