
#include "chess_net/server.h"


ABSL_FLAG(uint16_t, port, 5005, "Server port for the chess service");

std::mutex SERVER_MTX;

std::map<std::string, Room> ROOM_OPENED;

std::map<std::string, RoomLock> ROOM_CLOSED;

std::map<std::string, RoomStatus> ROOM_CLOSED_STATUS;

std::map<std::string, Game> ROOM_GAME;



class GameChessServiceImpl final: public GameChess::Service{


    Status PostRoom(ServerContext* context, const Room* request,
                    ServerWriter<RoomResult>* reply_writer) override {
        
        int room_created = 0;

        int match_found = 0; 

        int lapsed_sec = 0;

        Room new_r;

        RoomResult r_res;

        int match_timeout = request->match_timeout();

        if (match_timeout > MAX_TIMEOUT_SECONDS){
          match_timeout = MAX_TIMEOUT_SECONDS;
        }

        new_r.set_room_id(request->room_id());
        new_r.set_room_name(request->room_name());
        new_r.set_host_color(request->host_color());
        new_r.set_match_timeout(request->match_timeout());
        new_r.set_move_timeout(request->move_timeout());


        room_created = RoomRegister(&new_r);

        if(room_created < 1){

          r_res.set_status(TALK::ABORT);

          reply_writer->Write(r_res);

          return Status::CANCELLED;
        }

        auto t_start = std::chrono::high_resolution_clock::now();

        
        for (;;){

          r_res.set_status(TALK::WAIT);

          reply_writer->Write(r_res);

          lapsed_sec = GetTimeDiffSeconds(t_start);

          if (lapsed_sec > match_timeout){
            
            r_res.set_status(TALK::TIMEOUT);

            reply_writer->Write(r_res);

            return Status::CANCELLED;
          }

          match_found = MatchWatcher(&new_r, &r_res); 

          if(match_found == 1){

            r_res.set_status(TALK::MATCH);

            reply_writer->Write(r_res);

            break;
          }

        }


        return Status::OK;
    }

    Status JoinRoom(ServerContext* context, const Room* request,
                    ServerWriter<RoomResult>* reply_writer){

        int match_found = 0; 

        int lapsed_sec = 0;

        int match_timeout = request->match_timeout();

        if (match_timeout > MAX_TIMEOUT_SECONDS){
          match_timeout = MAX_TIMEOUT_SECONDS;
        }

        RoomResult r_res;

        auto t_start = std::chrono::high_resolution_clock::now();

        for(;;){

          r_res.set_status(TALK::WAIT);

          reply_writer->Write(r_res);

          lapsed_sec = GetTimeDiffSeconds(t_start);

          if (lapsed_sec > match_timeout){
            
            r_res.set_status(TALK::TIMEOUT);

            reply_writer->Write(r_res);

            return Status::CANCELLED;
          }

          match_found = MatchFinder(request, &r_res); 

          if(match_found == 1){

            r_res.set_status(TALK::MATCH);

            reply_writer->Write(r_res);

            break;
          }

        }


        return Status::OK;
    }

    Status MakeMoveThenGet(ServerContext* context, const Move* request,
                    ServerWriter<MoveRecord>* reply_writer) {

        Move req_mv = *request;

        MoveRecord mr_res;

        MoveResult mv_result;

        TALK authenticated;

        TALK move_accepted;

        int is_white = 0;

        authenticated = AuthIncomingMoveRequest(&req_mv);

        if(authenticated != TALK::AUTH){

          mv_result.set_success(false);

          mv_result.set_code(authenticated);

          std::string time_stamp_str = GetStringTimeNow();

          mv_result.set_resolve_time_stamp(time_stamp_str);

          *mr_res.mutable_result() = mv_result;

          reply_writer->Write(mr_res);

          Loggerln<std::string>("client authentication failed: " + std::to_string(authenticated));

          return Status::CANCELLED;
          
        }
        

        SERVER_MTX.lock();

        move_accepted = ChessMove(&is_white, &req_mv, &mr_res, &mv_result);

        SERVER_MTX.unlock();

        if(move_accepted != TALK::OKAY){

          reply_writer->Write(mr_res);

          Loggerln<std::string>("chess move not accepted: " + std::to_string(move_accepted));

          return Status::CANCELLED;

        }


        Loggerln<std::string>("chess move acceptd: field updated");

        Loggerln<std::string>("handing over turn to the opponent");

        TALK judge;

        for(;;){
          
          int hit = 0;

          MoveRecord watch_mr_res;
          MoveResult watch_mv_result;
          

          judge = WatchChessMove(&is_white, &req_mv, &watch_mr_res, &watch_mv_result);

          switch(judge){

            case TALK::WATCH:

              reply_writer->Write(mr_res);

              break;

            case TALK::TURN:

              hit = 1;

              reply_writer->Write(watch_mr_res);

              break;

            case TALK::ETIMEOUT:

              hit = -1;

              reply_writer->Write(watch_mr_res);

              break;

            case TALK::EABORT:

              hit = -1;

              reply_writer->Write(watch_mr_res);

              break;
            
            default:

              hit = -1;

              reply_writer->Write(watch_mr_res);

              break;

          }

          if(hit == 1){

            break;

          } else if(hit == -1){

            return Status::CANCELLED;

          }

        }

        return Status::OK;
    }

    Status MakeMoveThenFree(ServerContext* context, const Move* request,
                    MoveRecord* reply) override {

        Move req_mv = *request;

        MoveResult mv_result;

        TALK authenticated;

        TALK move_accepted;

        int is_white = 0;

        authenticated = AuthIncomingMoveRequest(&req_mv);

        if(authenticated != TALK::AUTH){

          mv_result.set_success(false);

          mv_result.set_code(authenticated);

          std::string time_stamp_str = GetStringTimeNow();

          mv_result.set_resolve_time_stamp(time_stamp_str);

          *reply->mutable_result() = mv_result;

          Loggerln<std::string>("client authentication failed: " + std::to_string(authenticated));

          return Status::CANCELLED;
          
        }
        

        SERVER_MTX.lock();

        move_accepted = ChessMove(&is_white, &req_mv, reply, &mv_result);

        SERVER_MTX.unlock();

        if(move_accepted != TALK::OKAY){

          Loggerln<std::string>("chess move not accepted: " + std::to_string(move_accepted));

          return Status::CANCELLED;

        }


        Loggerln<std::string>("chess move acceptd: field updated");

        Loggerln<std::string>("handing over turn to the opponent without stream");

        return Status::OK;

    }


    Status GetMoveRecord(ServerContext* context, const Get* request,
                    ServerWriter<MoveRecord>* reply_writer) {


        Get req_get = *request;

        MoveRecord mr_res;

        MoveResult mv_result;

        TALK authenticated;

        TALK get_accepted;

        int is_white = 0;

        authenticated = AuthIncomingGetRequest(&req_get);

        if(authenticated != TALK::AUTH){

          mv_result.set_success(false);

          mv_result.set_code(authenticated);

          std::string time_stamp_str = GetStringTimeNow();

          mv_result.set_resolve_time_stamp(time_stamp_str);

          *mr_res.mutable_result() = mv_result;

          reply_writer->Write(mr_res);

          Loggerln<std::string>("get client authentication failed: " + std::to_string(authenticated));

          return Status::CANCELLED;
          
        }
      

        get_accepted = ChessGet(&is_white, &req_get);


        if(get_accepted != TALK::OKAY){

          reply_writer->Write(mr_res);

          Loggerln<std::string>("chess get not accepted: " + std::to_string(get_accepted));

          return Status::CANCELLED;

        }

        Loggerln<std::string>("streaming get status");

        TALK judge;

        for(;;){
          
          int hit = 0;

          MoveRecord watch_mr_res;
          MoveResult watch_mv_result;
          
          judge = WatchChessGet(&is_white, &req_get, &watch_mr_res, &watch_mv_result);

          switch(judge){

            case TALK::WATCH:

              reply_writer->Write(mr_res);

              break;

            case TALK::TURN:

              hit = 1;

              reply_writer->Write(watch_mr_res);

              break;

            case TALK::ETIMEOUT:

              hit = -1;

              reply_writer->Write(watch_mr_res);

              break;

            case TALK::EABORT:

              hit = -1;

              reply_writer->Write(watch_mr_res);

              break;
            
            default:

              hit = -1;

              reply_writer->Write(watch_mr_res);

              break;

          }

          if(hit == 1){

            break;

          } else if(hit == -1){

            return Status::CANCELLED;

          }

        }

          

        return Status::OK;
    }

    Status GetMoveHistory(ServerContext* context, const Get* request,
                    MoveHistory* reply) override {

        Get req_get = *request;

        TALK authenticated;

        int status = 0;

        authenticated = AuthIncomingGetRequest(&req_get);

        if(authenticated != TALK::AUTH){

          Loggerln<std::string>("get mhist: client authentication failed: " + std::to_string(authenticated));

          return Status::CANCELLED;
          
        }

        std::string room_id = req_get.room_id();

        status = FillMoveHistoryByRoomId(room_id, reply);

        if(status < 0){

          return Status::CANCELLED;

        }


        return Status::OK;
        

    }


    Status Leave(ServerContext* context, const GG* request,
                    Report* reply) override {

        GG req_gg = *request;

        int success = 0;

        success = ConstructLeaveReport(&req_gg, reply);

        if(success < 1){
          
          Loggerln<std::string>("failed to construct leave message");
          
          return Status::CANCELLED;

        }

        Loggerln<std::string>("leave process successful");

        return Status::OK;

    }

    Status MakeMove(ServerContext* context, const Move* request,
                    MoveResult* reply) override {

        Move req_mv = *request;

        MoveRecord mrec;

        PrintReqMove(&req_mv);

        SetMoveResult(reply);

        std::cout << "set move result success " << std::endl; 

        SetMoveRecord(&mrec, &req_mv, reply);

        std::cout << "set move record success" << std::endl;

        int retcode = AddToMoveHistory(&mrec);

        if(retcode == 0){

          std::cout << "add to move history success" << std::endl;

          return Status::OK;


        } else {

          return Status::CANCELLED;
        }

    }


};


void RunServer(uint16_t port){

  std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
  GameChessServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;

  // tls
  grpc::SslServerCredentialsOptions ssl_opt;

  std::stringstream str_stream;

  std::string key;
  std::ifstream key_file;

  std::string cert;
  std::ifstream cert_file;

  std::string ca;
  std::ifstream ca_file;


  key_file.open("server.key");
  str_stream << key_file.rdbuf();
  key = str_stream.str();
  key_file.close();

  cert_file.open("server.pem");
  str_stream << cert_file.rdbuf();
  cert = str_stream.str();
  cert_file.close();

  ca_file.open("ca.pem");
  str_stream << ca_file.rdbuf();
  ca = str_stream.str();
  ca_file.close();

  grpc::SslServerCredentialsOptions::PemKeyCertPair keycert = {
    key,
    cert
  };

  
  ssl_opt.pem_root_certs = ca;
  ssl_opt.pem_key_cert_pairs.push_back(keycert);


  builder.AddListeningPort(server_address, grpc::SslServerCredentials(ssl_opt));

  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Game Chess Server listening on " << server_address << std::endl;

  server->Wait();

}



int main (int argc, char** argv){

  InitRandomDevice();

  absl::ParseCommandLine(argc, argv);
  RunServer(absl::GetFlag(FLAGS_port));
  return 0;
}