cmake_minimum_required(VERSION 3.8)

project(multi_terminal_games)

include(../common.cmake)

include(../chess/CMakeLists.txt)

# Proto file
get_filename_component(mtgms_proto "../../proto/mtgms.proto" ABSOLUTE)
get_filename_component(mtgms_proto_path "${mtgms_proto}" PATH)

# Generated sources
set(mtgms_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/mtgms.pb.cc")
set(mtgms_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/mtgms.pb.h")
set(mtgms_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/mtgms.grpc.pb.cc")
set(mtgms_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/mtgms.grpc.pb.h")
add_custom_command(
      OUTPUT "${mtgms_proto_srcs}" "${mtgms_proto_hdrs}" "${mtgms_grpc_srcs}" "${mtgms_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${mtgms_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${mtgms_proto}"
      DEPENDS "${mtgms_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# mtgms_grpc_proto
add_library(mtgms_grpc_proto
  ${mtgms_grpc_srcs}
  ${mtgms_grpc_hdrs}
  ${mtgms_proto_srcs}
  ${mtgms_proto_hdrs})
target_link_libraries(mtgms_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Targets terminal
foreach(_target
  mtgms_client
  mtgms_callback_client
  mtgms_async_client mtgms_async_client2)
  add_executable(${_target} "../../terminal/mtgms/${_target}.cc")
  target_link_libraries(${_target}
    mtgms_grpc_proto
    absl::flags
    absl::flags_parse
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()

# Target game
foreach(_target
  mtgms_server
  mtgms_callback_server
  mtgms_async_server)
  add_executable(${_target} "../../multigames/mtgms/${_target}.cc")
  target_link_libraries(${_target}
    mtgms_grpc_proto
    absl::flags
    absl::flags_parse
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()