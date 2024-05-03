if(STANDALONE_GAME)

  cmake_minimum_required(VERSION 3.8)

  project(chess)

  include(../common.cmake)

  # Proto file
  get_filename_component(chess_proto "../../proto/chess.v1.proto" ABSOLUTE)
  get_filename_component(chess_proto_path "${chess_proto}" PATH)

  # Generated sources
  set(chess_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/chess.v1.pb.cc")
  set(chess_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chess.v1.pb.h")
  set(chess_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/chess.v1.grpc.pb.cc")
  set(chess_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chess.v1.grpc.pb.h")
  add_custom_command(
        OUTPUT "${chess_proto_srcs}" "${chess_proto_hdrs}" "${chess_grpc_srcs}" "${chess_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
          --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
          -I "${chess_proto_path}"
          --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
          "${chess_proto}"
        DEPENDS "${chess_proto}")

  # Include generated *.pb.h files
  include_directories("${CMAKE_CURRENT_BINARY_DIR}")

  # chess_grpc_proto
  add_library(chess_grpc_proto
    ${chess_grpc_srcs}
    ${chess_grpc_hdrs}
    ${chess_proto_srcs}
    ${chess_proto_hdrs})
  target_link_libraries(chess_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
  
  # chess game

  set(CHESS_INCLUDE "../../include/chess")

  set(chess_game "../../multigames/chess/chess.cc")
  
  set(chess_interface "../../terminal/chess/user_interface.cc")
  set(chess_terminal "../../terminal/chess/main.cc")

  include_directories("${CHESS_INCLUDE}")


  # chess server

  add_library(gm_chess
    ${chess_game}
  )


  # chess client

  add_executable(chess
    ${chess_terminal}
    ${chess_interface}
  )
  target_link_libraries(chess
    gm_chess
  )

  # Targets terminal
  foreach(_target
    chess_client)
    add_executable(${_target} "../../terminal/chess/${_target}.cc")
    target_link_libraries(${_target}
      chess_grpc_proto
      absl::flags
      absl::flags_parse
      ${_REFLECTION}
      ${_GRPC_GRPCPP}
      ${_PROTOBUF_LIBPROTOBUF})
  endforeach()

  # Target game
  foreach(_target
    chess_server)
    add_executable(${_target} "../../multigames/chess/${_target}.cc")
    target_link_libraries(${_target}
      chess_grpc_proto
      absl::flags
      absl::flags_parse
      ${_REFLECTION}
      ${_GRPC_GRPCPP}
      ${_PROTOBUF_LIBPROTOBUF})
  endforeach()

elseif(BUILD_DEV)


  cmake_minimum_required(VERSION 3.8)

  project(chess_build_dev)

  include(../common.cmake)

  # Proto file
  get_filename_component(chess_proto "../../proto/chess.v1.proto" ABSOLUTE)
  get_filename_component(chess_proto_path "${chess_proto}" PATH)

  # Generated sources
  set(chess_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/chess.v1.pb.cc")
  set(chess_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chess.v1.pb.h")
  set(chess_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/chess.v1.grpc.pb.cc")
  set(chess_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chess.v1.grpc.pb.h")
  add_custom_command(
        OUTPUT "${chess_proto_srcs}" "${chess_proto_hdrs}" "${chess_grpc_srcs}" "${chess_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
          --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
          -I "${chess_proto_path}"
          --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
          "${chess_proto}"
        DEPENDS "${chess_proto}")

  # Include generated *.pb.h files
  include_directories("${CMAKE_CURRENT_BINARY_DIR}")

  # chess_grpc_proto
  add_library(chess_grpc_proto
    ${chess_grpc_srcs}
    ${chess_grpc_hdrs}
    ${chess_proto_srcs}
    ${chess_proto_hdrs})
  target_link_libraries(chess_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})


elseif(BUILD_DEV_THEN)

  cmake_minimum_required(VERSION 3.8)

  project(chess_build_dev)

  include(../common.cmake)

  set(CHESS_INCLUDE "../../include")

  include_directories(
    "${CHESS_INCLUDE}"
    ./dev
    )


  link_directories( 
    ./dev
    )

  # Targets terminal
  foreach(_target
    chess_client)
    add_executable(
      ${_target} 
      "../../terminal/chess/chess_client.cc"
      "../../terminal/chess/chess_req.cc"
      "../../terminal/chess/chess_client_utils.cc"
      )
    target_link_libraries(${_target}
      chess_grpc_proto
      absl::flags
      absl::flags_parse
      ${_REFLECTION}
      ${_GRPC_GRPCPP}
      ${_PROTOBUF_LIBPROTOBUF})
  endforeach()

  # Target game
  foreach(_target
    chess_server)
    add_executable(
      ${_target} 
      "../../multigames/chess/chess_server.cc"
      "../../multigames/chess/chess_admin.cc"
      "../../multigames/chess/chess_calc.cc"
      "../../multigames/chess/chess_utils.cc"
      "../../multigames/chess/v1/chess.cc"
      )
    target_link_libraries(${_target}
      chess_grpc_proto
      absl::flags
      absl::flags_parse
      ${_REFLECTION}
      ${_GRPC_GRPCPP}
      ${_PROTOBUF_LIBPROTOBUF})
  endforeach() 
    

elseif(BUILD_SAMPLE)


  cmake_minimum_required(VERSION 3.8)

  project(chess_build_sample)

  include(../common.cmake)

  # Proto file
  get_filename_component(chess_proto "../../proto/chess.v0.proto" ABSOLUTE)
  get_filename_component(chess_proto_path "${chess_proto}" PATH)

  # Generated sources
  set(chess_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/chess.v0.pb.cc")
  set(chess_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/chess.v0.pb.h")
  add_custom_command(
        OUTPUT "${chess_proto_srcs}" "${chess_proto_hdrs}" "${chess_grpc_srcs}" "${chess_grpc_hdrs}"
        COMMAND ${_PROTOBUF_PROTOC}
        ARGS 
          --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
          -I "${chess_proto_path}"
          "${chess_proto}"
        DEPENDS "${chess_proto}")

  # Include generated *.pb.h files
  include_directories("${CMAKE_CURRENT_BINARY_DIR}")

  # chess_proto
  add_library(chess_proto
    ${chess_proto_srcs}
    ${chess_proto_hdrs})
  target_link_libraries(chess_proto
    ${_PROTOBUF_LIBPROTOBUF})


elseif(BUILD_SAMPLE_THEN)

  cmake_minimum_required(VERSION 3.8)

  project(chess_build_sample)

  include(../common.cmake)


  include_directories(
    ./sample
    )


  link_directories( 
    ./sample
    )

  # Targets terminal
  foreach(_target
    chess_sample)
    add_executable(${_target} "../../sample/${_target}.cc")
    target_link_libraries(${_target}
      chess_proto
      ${_PROTOBUF_LIBPROTOBUF})
  endforeach()


else() # If STANDALONE_GAME is not set




endif()