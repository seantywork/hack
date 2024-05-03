#include "chess_net/server.h"




TALK ChessMove(int* is_white, Move* req_mv, MoveRecord* mr_res, MoveResult* mv_result){

  TALK ret_status;

  std::string room_id;

  std::string key;

  PIECES piece_id;

  std::string move_to;

  room_id = req_mv->room_id();

  key = req_mv->key();

  piece_id = req_mv->id();

  move_to = req_mv->to();



  int is_poster_key = 0;

  int is_white_piece = 0;

  int opening = 0;

  int white_turn = 0;

  
  int check = GetKeyContextInfoByRoomId(room_id, key, &is_poster_key, &is_white_piece, &opening, &white_turn);

  *is_white = is_white_piece;

  if(check < 0){

    Loggerln<std::string>("failed to get key context by room id: " + std::to_string(check));

    return TALK::ENTOK;

  }

  if(opening == 1){
    
    Loggerln<std::string>("opening move for: room_id: " + room_id + " key: " + key);

  }

  if(white_turn == 0 && is_white_piece == 1){

    Loggerln<std::string>("black turn, but white tried to move");

    return TALK::ENTOK;


  } else if(white_turn == 1 && is_white_piece == 0){

    Loggerln<std::string>("white turn, but black tried to move");

    return TALK::ENTOK;

  }


  Loggerln<std::string>("accessing game class for: " + room_id);

  Game* gm = &ROOM_GAME[room_id];

  /*
  TODO:
    calculate actual move on the board using the engine
  */

  Loggerln<std::string>("exiting game class for: " + room_id);  

  mv_result->set_success(true);

  mv_result->set_code(TALK::OKAY);

  std::string rt_temp = GetStringTimeNow();

  mv_result->set_resolve_time_stamp(rt_temp);

  mr_res->set_id(piece_id);

  mr_res->set_to(move_to);

  *mr_res->mutable_result() = *mv_result;

  UpdateStepInfoByRoomId(room_id, mr_res);

  ret_status = TALK::OKAY;


  return ret_status;
}


TALK ChessGet(int* is_white, Get* req_get){


  TALK ret_status;

  std::string room_id;

  std::string key;

  room_id = req_get->room_id();

  key = req_get->key();

  int is_poster_key = 0;

  int is_white_piece = 0;

  int opening = 0;

  int white_turn = 0;

  
  int check = GetKeyContextInfoByRoomId(room_id, key, &is_poster_key, &is_white_piece, &opening, &white_turn);

  *is_white = is_white_piece;

  if(check < 0){

    Loggerln<std::string>("failed to get key context by room id: " + std::to_string(check));

    return TALK::ENTOK;

  }

  ret_status = TALK::OKAY;

  return ret_status;
}



int GetKeyContextInfoByRoomId(std::string room_id, std::string key, int* is_poster, int* is_white, int* opening, int* white_turn){


  RoomLock* rl = &ROOM_CLOSED[room_id];
  
  RoomStatus* rs = &ROOM_CLOSED_STATUS[room_id];    

  std::string log_str;

  *opening = 0;

  if(rl->poster_key() == key){

      *is_poster = 1;

  } else if(rl->joiner_key() == key){

      *is_poster = 0;
  
  } 


  Room* r = rl->mutable_r();

  SIDE host_color = r->host_color();

  if(host_color == SIDE::WHITE){

      if(*is_poster == 1){

          *is_white = 1;

      } else {

          *is_white = 0;

      }

  } else {
      
      if(*is_poster == 1){

          *is_white = 0;

      } else {

          *is_white = 1;

      }

  }


  MoveHistory* mv_hist = rs->mutable_move_history();
  
  int mv_hist_size = mv_hist->move_history_size();

  if(mv_hist_size == 0){

      *opening = 1;

      *white_turn = 1;

  }


  if(*opening == 1){
      
      return 0;
  }


  MoveRecord* mv_last = rs->mutable_move_last();

  PIECES piece_id = mv_last->id();

  int piece_id_int = (int)piece_id;

  if(piece_id_int < 15){

      *white_turn = 0;
          
  } else {

      *white_turn = 1;

  }


  return 1;

}


void UpdateStepInfoByRoomId(std::string room_id, MoveRecord* mr){


  Loggerln<std::string>("getting step info for room: " + room_id);


  RoomStatus* rs = &ROOM_CLOSED_STATUS[room_id];

  MoveHistory* mv_hist = rs->mutable_move_history();

  int mv_hist_length = mv_hist->move_history_size();


  int new_step;

  if(mv_hist_length == 0){

    Loggerln<std::string>("move history is 0, opening");

    new_step = 0;

    mr->set_step(new_step);

  } else {

    Loggerln<std::string>("updating last move");

    int last_step = rs->move_last().step();

    new_step = last_step + 1;

    mr->set_step(new_step);
  }

  *rs->mutable_move_last() = *mr;

  Loggerln<std::string>("append to move history: " + std::to_string(mv_hist_length));

  MoveRecord* new_mv_record = mv_hist->add_move_history();

  new_mv_record->set_step(new_step);

  new_mv_record->set_id(mr->id());

  new_mv_record->set_to(mr->to());

  *new_mv_record->mutable_result() = *mr->mutable_result();

  mv_hist_length = mv_hist->move_history_size();

  Loggerln<std::string>("successfully appended to move history: " + std::to_string(mv_hist_length));

  return;
}


TALK WatchChessMove(int* is_white, Move* req_mv, MoveRecord* watch_mr_res, MoveResult* watch_mv_result){


  TALK ret_status;

  std::string room_id;

  room_id = req_mv->room_id();

  int is_white_piece = *is_white;

  int is_last_white = 0;

  RoomStatus* rs = &ROOM_CLOSED_STATUS[room_id];

  MoveRecord* mv_last = rs->mutable_move_last();

  int step_num = mv_last->step();

  PIECES piece_id = mv_last->id();

  std::string move_to = mv_last->to();

  MoveResult* mv_result = mv_last->mutable_result();

  bool success_stat = mv_result->success();

  TALK code_stat = mv_result->code();

  std::string last_time_stamp = mv_result->resolve_time_stamp();


  if(piece_id < 15){
    
    is_last_white = 1;

  } else {

    is_last_white = 0;

  }

  if(
    (is_white_piece == 1 && is_last_white == 1)
    || (is_white_piece == 0 && is_last_white == 0)){

    // wait and check for timeout and everything else

    ret_status = TALK::WATCH;

  } else if (
    (is_white_piece == 1 && is_last_white == 0)
    || (is_white_piece == 0 && is_last_white == 1)){

    watch_mv_result->set_success(success_stat);

    watch_mv_result->set_code(code_stat);

    watch_mv_result->set_resolve_time_stamp(last_time_stamp);

    *watch_mr_res->mutable_result() = *watch_mv_result;

    watch_mr_res->set_to(move_to);

    watch_mr_res->set_id(piece_id);

    watch_mr_res->set_step(step_num);
  

    ret_status = TALK::TURN;

  }


  return ret_status;

}


TALK WatchChessGet(int* is_white, Get* req_get, MoveRecord* watch_mr_res, MoveResult* watch_mv_result){


  TALK ret_status;

  std::string room_id;

  room_id = req_get->room_id();

  int is_white_piece = *is_white;

  int is_last_white = 0;

  RoomStatus* rs = &ROOM_CLOSED_STATUS[room_id];

  MoveRecord* mv_last = rs->mutable_move_last();

  int step_num = mv_last->step();

  PIECES piece_id = mv_last->id();

  std::string move_to = mv_last->to();

  MoveResult* mv_result = mv_last->mutable_result();

  bool success_stat = mv_result->success();

  TALK code_stat = mv_result->code();

  std::string last_time_stamp = mv_result->resolve_time_stamp();

  MoveHistory* mv_hist = rs->mutable_move_history();

  int mv_hist_size = mv_hist->move_history_size();

  if(mv_hist_size == 0){

    is_last_white = 0;

  } else {

    if(piece_id < 15){
    
        is_last_white = 1;

    } else {

        is_last_white = 0;

    }
  }


  if(
    (is_white_piece == 1 && is_last_white == 1)
    || (is_white_piece == 0 && is_last_white == 0)){

    // wait and check for timeout and everything else

    ret_status = TALK::WATCH;

  } else if (
    (is_white_piece == 1 && is_last_white == 0)
    || (is_white_piece == 0 && is_last_white == 1)){

    watch_mv_result->set_success(success_stat);

    watch_mv_result->set_code(code_stat);

    watch_mv_result->set_resolve_time_stamp(last_time_stamp);

    *watch_mr_res->mutable_result() = *watch_mv_result;

    watch_mr_res->set_to(move_to);

    watch_mr_res->set_id(piece_id);

    watch_mr_res->set_step(step_num);
  

    ret_status = TALK::TURN;

  }


  return ret_status;

}


int FillMoveHistoryByRoomId(std::string room_id, MoveHistory* mhist){


  int ret_status = 0;

  if(ROOM_CLOSED_STATUS.find(room_id) == ROOM_CLOSED_STATUS.end()){

    Loggerln<std::string>("fill mhist: couldn't find room lock status for room id: " + room_id);

    return -1;

  }

  RoomStatus* rs = &ROOM_CLOSED_STATUS[room_id];

  MoveHistory* rs_mhist = rs->mutable_move_history();

  int rs_mhist_size = rs_mhist->move_history_size();

  for(int i = 0 ; i < rs_mhist_size; i++){

    MoveRecord* mv_rec = mhist->add_move_history();

    MoveRecord* rs_mhist_mv_rec = rs_mhist->mutable_move_history(i);

    mv_rec->set_step(rs_mhist_mv_rec->step());

    mv_rec->set_id(rs_mhist_mv_rec->id());

    mv_rec->set_to(rs_mhist_mv_rec->to());

    MoveResult* mv_res;

    MoveResult* rs_mhist_mv_res = rs_mhist_mv_rec->mutable_result();

    mv_res->set_success(rs_mhist_mv_res->success());

    mv_res->set_code(rs_mhist_mv_res->code());

    mv_res->set_resolve_time_stamp(rs_mhist_mv_res->resolve_time_stamp());

    *mv_rec->mutable_result() = *mv_res;


  }

  ret_status = 0;


  return ret_status;

}

void PrintReqMove(Move* mv){


  int piece_id = mv->id();
  std::string to = mv->to();

  std::cout<< "client piece id: " << piece_id << std::endl;
  std::cout<< "client move: " << to << std::endl;

}

void SetMoveResult(MoveResult* mr){

  mr->set_success(true);
  mr->set_resolve_time_stamp(GetStringTimeNow());

}


void SetMoveRecord(MoveRecord* mrec, Move* mv, MoveResult* mr){

  int step = 0;
  PIECES id = mv->id();
  std::string to = mv->to();

  mrec->set_step(step);
  mrec->set_id(id);
  mrec->set_to(to);
  *mrec->mutable_result() = *mr;

}


int AddToMoveHistory(MoveRecord* mrec){


  MoveHistory mhist;

  std::string bin_path = "bin/history.bin";

  std::fstream input(bin_path, std::ios::in | std::ios::binary);

  if(!input){
    std::cout << "Creating new at: " << bin_path << std::endl; 
  } else if (!mhist.ParseFromIstream(&input)) {
    std::cerr << "Failed parsing from :" << bin_path << std::endl;
    return -1;
  }

  auto new_mrec = mhist.add_move_history();

  new_mrec->set_step(mrec->step());
  new_mrec->set_id(mrec->id());
  new_mrec->set_to(mrec->to());
  *new_mrec->mutable_result() = *mrec->mutable_result();

  std::fstream output(bin_path, std::ios::out | std::ios::trunc | std::ios::binary);
  if(!mhist.SerializeToOstream(&output)){
    std::cerr << "Failed writing to: " << bin_path << std::endl;
    return -1;
  }

  return 0;
}