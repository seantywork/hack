
#include "chess_net/client.h"


std::string GetStringTimeNow(){



    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    std::string ts = oss.str();


    return ts;

}
