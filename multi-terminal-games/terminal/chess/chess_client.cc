
#include "chess_net/client.h"

ABSL_FLAG(std::string, target, "localhost:5005", "Server address");



class GameChessClient{
public:

  GameChessClient(std::shared_ptr<Channel> channel):
    stub_(GameChess::NewStub(channel)) {}

  int PostRoom(std::string room_name, SIDE host_color, int match_timeout, int move_timeout, std::string* key, std::string* room_id){

    int post_result = 0;

    ClientContext context;

    Room request;

    request.set_room_name(room_name);

    request.set_host_color(host_color);

    request.set_match_timeout(match_timeout);

    request.set_move_timeout(move_timeout);

    RoomResult reply_written;

    std::unique_ptr<ClientReader<RoomResult>> reader(
      stub_->PostRoom(&context, request)
    );

    std::cout << "receiving message *** " << std::endl;

    while(reader->Read(&reply_written)){ 
      continue;
    }

    std::cout << "received message *** " << std::endl;

    std::cout << "status: " <<  reply_written.status() << std::endl;

    std::cout << "key: " <<  reply_written.key() << std::endl;

    Room* resp_r = reply_written.mutable_r();

    std::cout << "room_id: " << resp_r->room_id() << std::endl;

    std::cout << "room_name: " << resp_r->room_name() << std::endl;

    std::cout << "host_color: " << resp_r->host_color() << std::endl;

    std::cout << "match_timeout: " << resp_r->match_timeout() << std::endl;

    std::cout << "move_timeout: " << resp_r->move_timeout() << std::endl; 

    Status status = reader->Finish();

    if(status.ok()){
    
      post_result = 1;


      *key = reply_written.key();

      *room_id = resp_r->room_id();

      Loggerln<std::string>("room post successful");
    
    } else{
    
      Loggerln<std::string>("room post failed");
    
    }

    return post_result;

  }

  int JoinRoom(std::string room_name, SIDE host_color, int match_timeout, int move_timeout,std::string* key, std::string* room_id){

    int join_result = 0;

    ClientContext context;

    Room request;

    request.set_room_name(room_name);

    request.set_host_color(host_color);

    request.set_match_timeout(match_timeout);

    request.set_move_timeout(move_timeout);

    RoomResult reply_written;

    std::unique_ptr<ClientReader<RoomResult>> reader(
      stub_->JoinRoom(&context, request)
    );

    std::cout << "waiting for a room to join *** " << std::endl;

    while(reader->Read(&reply_written)){ 
      continue;
    }
  
    std::cout << "received message *** " << std::endl;

    std::cout << "status: " <<  reply_written.status() << std::endl;

    std::cout << "key: " <<  reply_written.key() << std::endl;

    Room* resp_r = reply_written.mutable_r();

    std::cout << "room_id: " << resp_r->room_id() << std::endl;

    std::cout << "room_name: " << resp_r->room_name() << std::endl;

    std::cout << "host_color: " << resp_r->host_color() << std::endl;

    std::cout << "match_timeout: " << resp_r->match_timeout() << std::endl;

    std::cout << "move_timeout: " << resp_r->move_timeout() << std::endl;

    Status status = reader->Finish();

    if(status.ok()){
    
      join_result = 1;

      *key = reply_written.key();

      *room_id = resp_r->room_id();

      
      Loggerln<std::string>("joined");
    
    } else{
    
      Loggerln<std::string>("failed to join");
    
    }

    return join_result; 
  }
  

  int MakeMoveThenGet(std::string key, std::string room_id, PIECES id, std::string to){

    Move request;

    request.set_room_id(room_id);
    request.set_key(key);
    request.set_id(id);
    request.set_to(to);



    int mkmv_then_get_result = 0;

    ClientContext context;

    MoveRecord reply_written;

    std::unique_ptr<ClientReader<MoveRecord>> reader(
      stub_->MakeMoveThenGet(&context, request)
    );

    std::cout << "waiting for the opponent move *** " << std::endl;

    while(reader->Read(&reply_written)){ 
      continue;
    }

    std::cout << "received message *** " << std::endl;
    
    std::cout << "step: " << reply_written.step() << std::endl;

    std::cout << "id: " << reply_written.id() << std::endl;

    std::cout << "to: " << reply_written.to() << std::endl;

    MoveResult* mv_res = reply_written.mutable_result();

    std::cout << "success: " << mv_res->success() << std::endl;

    std::cout << "code : " << mv_res->code() << std::endl;

    std::cout << "resolve time stamp: " << mv_res->resolve_time_stamp() << std::endl;

    Status status = reader->Finish();

    if(status.ok()){
      mkmv_then_get_result = 1;
      Loggerln<std::string>("move made");
    } else{
      Loggerln<std::string>("failed to make move");
    }

    return mkmv_then_get_result;

  }


  int GetMoveRecord(std::string key, std::string room_id){

    int get_mv_rec_result = 0;

    Get request;

    request.set_key(key);

    request.set_room_id(room_id);

    ClientContext context;

    MoveRecord reply_written;

    std::unique_ptr<ClientReader<MoveRecord>> reader(
      stub_->GetMoveRecord(&context, request)
    );

    std::cout << "getting move record *** " << std::endl;

    while(reader->Read(&reply_written)){ 
      continue;
    }

    std::cout << "received message *** " << std::endl;
    
    std::cout << "step: " << reply_written.step() << std::endl;

    std::cout << "id: " << reply_written.id() << std::endl;

    std::cout << "to: " << reply_written.to() << std::endl;

    MoveResult* mv_res = reply_written.mutable_result();

    std::cout << "success: " << mv_res->success() << std::endl;

    std::cout << "code : " << mv_res->code() << std::endl;

    std::cout << "resolve time stamp: " << mv_res->resolve_time_stamp() << std::endl;

    Status status = reader->Finish();

    if(status.ok()){
      get_mv_rec_result = 1;
      Loggerln<std::string>("got move record");
    } else{
      Loggerln<std::string>("failed to get record");
    }


    return get_mv_rec_result;
  }


  int GetMoveHistory(std::string key, std::string room_id, MoveHistory* mhist){

    int get_mhist_result = 0;

    Get request;

    request.set_key(key);

    request.set_room_id(room_id);


    ClientContext context;

    Status status = stub_->GetMoveHistory(&context, request, mhist);


    if(status.ok()){

      get_mhist_result = 1;

    } else {

      get_mhist_result = -1;

    }




    return get_mhist_result;
  }


private:

  std::unique_ptr<GameChess::Stub> stub_;

};



int main (int argc, char** argv){

  absl::ParseCommandLine(argc, argv);
  std::string target_str = absl::GetFlag(FLAGS_target);


  std::stringstream str_stream;
/*
  std::string key;
  std::ifstream key_file;

  std::string cert;
  std::ifstream cert_file;
*/
  std::string ca;
  std::ifstream ca_file;

/*
  key_file.open("server.key");
  str_stream << key_file.rdbuf();
  key = str_stream.str();
  key_file.close();

  cert_file.open("server.pem");
  str_stream<< cert_file.rdbuf();
  cert = str_stream.str();
  cert_file.close();
*/
  ca_file.open("ca.pem");
  str_stream << ca_file.rdbuf();
  ca = str_stream.str();
  ca_file.close();

  grpc::SslCredentialsOptions ssl_opt = {
    ca
  };


  GameChessClient gcc(
    grpc::CreateChannel(target_str, grpc::SslCredentials (ssl_opt)));


  std::string target = argv[1];
  std::string room_name = argv[2];
  std::string side_str = argv[3];
  std::string key;
  std::string room_id;

  SIDE side;

  if(side_str == "black"){

    side = SIDE::BLACK;
  
  } else if(side_str == "white"){

    side = SIDE::WHITE;

  } else {

    std::cout << "wrong side: " << side_str << std::endl;

    return -1;
  }

  int ret_code = 0;


  // TODO:
  //   add resume

  if(target == "post"){

    ret_code = gcc.PostRoom(room_name, side, 30, 30, &key, &room_id);
    
  } else if(target == "join"){

    ret_code = gcc.JoinRoom(room_name, side, 30, 30, &key, &room_id);

  } else {

    std::cout << "wrong argument: " << target << std::endl;

    return -2;

  }

  if(ret_code < 0){

    std::cout << "failed: " << target << std::endl;

    return -3;
  }


  ret_code = gcc.GetMoveRecord(key, room_id);

  if(ret_code < 0){

    std::cout << "failed get move record: " << ret_code << std::endl;

    return -4;
  }


  char* delim = " ";

  for(;;){

    int index = 0;
    char cmd[MAX_CMDLINE_LEN] = {0};

    std::string id_str;
    int id_int;
    PIECES id;
    std::string to;

    printf("[ piece id ] [ move ]: ");

    fgets(cmd, MAX_CMDLINE_LEN, stdin);

    int get_strlen = 0;

    get_strlen = strlen(cmd);

    for (int i = 0 ; i < get_strlen; i++){

      if(cmd[i] == '\n'){
        cmd[i] = '\0';
      }

    }

    char* ptk;

    ptk = strtok(cmd, delim);

    while (ptk != NULL){

      if(index == 0){

        id_str = ptk;
        id_int = stoi(id_str);
        id = (PIECES)id_int;

      }

      if (index == 1){

        to = ptk;

      }

      ptk = strtok(NULL, delim);

      index += 1;
    }

    

    gcc.MakeMoveThenGet(key, room_id, id, to);


  }


  return 0;

}