
#include "frank/controller/index.h"

int s_sig_num = 0;
static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  s_sig_num = sig_num;
}


int main(){

    srand((unsigned int)time(NULL));

    int ogapi = load_og_api_info();

    if(ogapi < 0){

        printf("failed to read og api \n");

        return -1;
    }
    

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    mg_log_set(MG_LL_INFO);  

    frankc_listen_and_serve();
 
    MG_INFO(("exiting on signal %d", s_sig_num));

    

    return 0;
}