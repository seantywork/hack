
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <iomanip>
#include <ctime>

#include "chess.v0.pb.h"

using gmchess::PIECES;
using gmchess::Test;
using gmchess::Void;
using gmchess::Move;
using gmchess::MoveResult;
using gmchess::MoveHistory;


std::string GetStringTimeNow(){



    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
    std::string ts = oss.str();


    return ts;


}


std::string SerializeExample(){

    auto t = std::make_shared<Test>();

    std::string target_name = "abcd";

    std::vector<std::string> target_names;

    target_names.push_back("ccc");

    target_names.push_back("ddd");

    target_names.push_back("fff");


    t->set_id(PIECES::WHITE_QUEEN);
    t->set_name(target_name);
    
    Move sm;
    MoveResult r;

    std::string outer_name = "hijk";

    sm.set_id(PIECES::WHITE_KING);
    sm.set_to(outer_name);

    std::string ts = GetStringTimeNow();

    r.set_success(true);
    r.set_resolve_time_stamp(ts);

    *sm.mutable_result() = r;


    *t->mutable_single_move() = sm;



    // set repeated values

    for (int i = 0 ; i < target_names.size() ; i ++){

        t->add_multiple_names(target_names[i]);

    }


    for (int i = 0 ; i < 3; i ++){

        Move* mv = t->add_multiple_moves();
        MoveResult mr;


        std::string inner_name = "xyz";
        mv->set_id(PIECES::BLACK_KING);
        mv->set_to(inner_name);

        std::string ts = GetStringTimeNow();

        mr.set_success(false);
        mr.set_resolve_time_stamp(ts);

        *mv->mutable_result() = mr;

    }


    std::string s = t->SerializeAsString();

    if(s.size() != 0){

        std::cout << "serialization success!" << std::endl;

    }else{

        std::cout << "serialization failed" << std::endl;
    }

    return s;


}



void DeserializeExample(std::string s){




    auto t = std::make_shared<Test>();

    bool check = t->ParseFromString(s);

    if(check){

        std::cout << "parsed successfully" << std::endl;
    } else {
        std::cout << "parsing failed" << std::endl;

    }

    std::cout<< "Test" << std::endl;
    std::cout<< "- id: " << t->id() << std::endl;
    std::cout<< "- name: " << t->name() << std::endl;
    
    auto sm = t->single_move();

    std::cout << "  SingleMove" << std::endl;
    
    std::cout << "  - id: " << sm.id() << std::endl;
    std::cout << "  - to: " << sm.to() << std::endl;

    auto mr = sm.result();

    std::cout << "    MoveResult" << std::endl;
    std::cout << "    - success: "  << mr.success() << std::endl;
    std::cout << "    - resolve_time_stamp: " << mr.resolve_time_stamp() << std::endl;


    int mn_size = t->multiple_names_size();

    for(int i = 0 ; i < mn_size; i ++){

        std::cout << "[name]: " << *t->mutable_multiple_names(i) << std::endl;


    }

    int mm_size = t->multiple_moves_size();

    for(int i = 0 ; i < mm_size; i ++){

        auto m = t->mutable_multiple_moves(i);    

        auto id = m->id();    
        auto to = m->to();

        auto mr = m->mutable_result();

        auto success = mr->success();
        auto resolve_time_stamp = mr->resolve_time_stamp();

        std::cout << "{Move}: id: " << id << " to: " << to <<  " MoveResult: success: " << success << " resolve_time_stamp: " << resolve_time_stamp << std::endl;
    }
    



}



int main (){

    std::string serialized = SerializeExample();

    DeserializeExample(serialized);

    
    return 0;


}