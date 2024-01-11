cmake_minimum_required(VERSION 3.5)
project(agv_cancomm)

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


if(AGVCANCOMM_DEBUG)
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

  set (CANCOMM hardware/agvcancomm)

  set (DRV_SRC hardware/agvcancomm/CANopenLinux)

  set (CANOPEN_SRC hardware/agvcancomm/CANopenLinux/CANopenNode)

  set (APPL_SRC hardware/agvcancomm/CANopenLinux/CANopenNode/example)

endif()

file(
  GLOB agvcancomm_cancomm_src
  "${CANCOMM}/cancomm.c"
)


file(
  GLOB agvcancomm_drv_src
  "${DRV_SRC}/CO_driver.c"
  "${DRV_SRC}/CO_epoll_interface.c"
  "${DRV_SRC}/CO_error.c"
  "${DRV_SRC}/CO_storageLinux.c"

)

file(
  GLOB agvcancomm_canopen_src1
  "${CANOPEN_SRC}/301/*.c"
)

file(
  GLOB agvcancomm_canopen_src3
  "${CANOPEN_SRC}/303/*.c"
)

file(
  GLOB agvcancomm_canopen_src4
  "${CANOPEN_SRC}/304/*.c"
)

file(
  GLOB agvcancomm_canopen_src5
  "${CANOPEN_SRC}/305/*.c"
)

file(
  GLOB agvcancomm_canopen_src9
  "${CANOPEN_SRC}/309/*.c"
)

file(
  GLOB agvcancomm_canopen_srcstorage
  "${CANOPEN_SRC}/storage/CO_storage.c"
)

file(
  GLOB agvcancomm_canopen_srcextra
  "${CANOPEN_SRC}/extra/CO_trace.c"
)

file(
  GLOB agvcancomm_canopen_srcroot
  "${CANOPEN_SRC}/CANopen.c"
)


file(
  GLOB agvcancomm_appl_src
  "${APPL_SRC}/OD.c"
)

file(
  GLOB agvcancomm_cancomm_header
  "${CANCOMM}/*.h"
)


file(
  GLOB agvcancomm_drv_header
  "${DRV_SRC}/*.h"
)

file(
  GLOB agvcancomm_canopen_header
  "${CANOPEN_SRC}/*.h"
)

file(
  GLOB agvcancomm_appl_header
  "${APPL_SRC}/*.h"
)


set(
  AGVCC_SRC 
  ${agvcancomm_cancomm_src}
  ${agvcancomm_drv_src} 
  ${agvcancomm_canopen_src1}
  ${agvcancomm_canopen_src3}
  ${agvcancomm_canopen_src4}
  ${agvcancomm_canopen_src5}
  ${agvcancomm_canopen_src9}
  ${agvcancomm_canopen_srcstorage}
  ${agvcancomm_canopen_srcextra}
  ${agvcancomm_canopen_srcroot}
  ${agvcancomm_appl_src}
  )

set(
  AGVCC_HEADER 
  ${agvcancomm_cancomm_header}
  ${agvcancomm_drv_header} 
  ${agvcancomm_canopen_header} 
  ${agvcancomm_appl_header}
  )


if(AGVCANCOMM_ELF)
  add_executable(
    agvcancomm
    "${AGVCC_SRC}"
  )
else()
  add_library(
    agvcancomm
    SHARED
    "${AGVCC_SRC}"
  )

  target_link_libraries(
    agvcancomm
    PUBLIC
    Threads::Threads
  )
endif()



target_include_directories(
  agvcancomm
  PRIVATE
  "${CANCOMM}"
  "${DRV_SRC}"
  "${CANOPEN_SRC}"
  "${APPL_SRC}"
)

install(
  FILES ${AGVCC_HEADER}
  DESTINATION include/agvcancomm
)

if(AGVCANCOMM_ELF)
  install(
    TARGETS agvcancomm
    RUNTIME DESTINATION /usr/bin
  )
else()
  install(
    TARGETS agvcancomm
    EXPORT agvcancomm
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
  )
endif()

if(NOT DEFINED STANDALONE_BUILD)
  install(
    EXPORT agvcancomm
    DESTINATION lib
    FILE agvcancomm-Config.cmake
    )
endif()
