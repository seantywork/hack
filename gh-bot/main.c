#include "bot/bot.c"


int main(){

    srand(time(0)); 

    while(1){
        gh_bot_iteration();
    }

    return 0;


}