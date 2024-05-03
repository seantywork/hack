
#include "chess_net/server.h"


std::random_device RD;
std::mt19937 RANDEV(RD());

void InitRandomDevice(){

    auto curr_clock = std::chrono::high_resolution_clock::now();
    std::seed_seq sseq {
        std::chrono::duration_cast<std::chrono::hours> (curr_clock.time_since_epoch()).count(), 
        std::chrono::duration_cast<std::chrono::seconds> (curr_clock.time_since_epoch()).count()
        };
    
    RANDEV.seed(sseq);
}

std::string GetStringTimeNow(){



    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string ts = oss.str();


    return ts;

}


int GetTimeDiffSeconds(std::chrono::time_point<std::chrono::high_resolution_clock> t_start){

  auto t_now = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> d_double;

  d_double = t_now - t_start;

  std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(d_double);

  return sec.count();

}

std::string GenRandomHexString(int keysize){

    std::string ret;

    std::ostringstream oss;

    std::uniform_int_distribution<int> dis(0x0, 0xF);
    
    
    oss << std::hex;

    for (std::size_t i = 0; i < keysize; ++i) {
	    oss << dis(RANDEV);
    }

    ret = oss.str();

    return ret;
}