#ifndef _CHESS_GRPC_CLIENT_H_
#define _CHESS_GRPC_CLIENT_H_

#include <condition_variable>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>

#include <fstream> 

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#include <grpcpp/grpcpp.h>


#include "chess.v1.grpc.pb.h"

#define MAX_CMDLINE_LEN 1024

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using gmchess::GameChess;

using gmchess::TALK;
using gmchess::SIDE;
using gmchess::PIECES;

using gmchess::Room;
using gmchess::RoomResult;


using gmchess::Void;

using gmchess::Get;
using gmchess::Move;
using gmchess::MoveRecord;
using gmchess::MoveResult;
using gmchess::MoveHistory;

std::string GetStringTimeNow();



template <typename T> void Loggerln(T msg){


    std::string now_str = GetStringTimeNow();

    now_str = "[ " + now_str + " ] ";


    std::cout << now_str <<  msg << std::endl;


}

#endif