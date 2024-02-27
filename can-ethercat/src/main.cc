/****************************************************************************/
#include "ecat_node.h"
/****************************************************************************/
#include "ecat_lifecycle.h"

//#include "control_logic_of_yours.h"

void signalHandler();
void SetRealTimeSettings();


std::shared_ptr<EthercatLifeCycleNode::EthercatLifeCycle> ECAT_LIFECYCLE_NODE;

int main(int argc, char **argv)
{
  SetRealTimeSettings();
  std::shared_ptr<EthercatLifeCycleNode::EthercatLifeCycle> ecat_lifecycle_node;
  ecat_lifecycle_node = std::make_shared<EthercatLifeCycleNode::EthercatLifeCycle>();

  //EthercatLifeCycleNode::EthercatLifeCycle* ecat_lifecycle_node;

  ECAT_LIFECYCLE_NODE = ecat_lifecycle_node;

#if HOMING_AT_START

  ecat_lifecycle_node->zeroed_all_ = 0;
  ecat_lifecycle_node->homed_all_ = 0;


#else

  ecat_lifecycle_node->zeroed_all_ = 1;
  ecat_lifecycle_node->homed_all_ = 1;


#endif

  if(ecat_lifecycle_node->on_configure())
  {
    return -1;
  }
  if(ecat_lifecycle_node->on_activate())
  {
    return -1;
  }

  int bogus_block = 0;
  printf("your control logic below: ");
  scanf("%d", &bogus_block);

/*
  int server_stat = 0 ;


  server_stat = ListenAndServe(SOCK_PORT);


  if(server_stat < 0 ){
    fLog<std::string>("server failure");

    ECAT_LIFECYCLE_NODE->on_shutdown();

    exit(EXIT_FAILURE);
  }

*/
  return 0;
}

void signalHandler(int /*signum*/){
    sig = 0;
    nanosleep((const struct timespec[]){0,g_kNsPerSec},NULL);
    ECAT_LIFECYCLE_NODE->on_shutdown();
    exit(0);
}

void SetRealTimeSettings()
{
      // Configure stdout sream buffer. _IONBF means no buffering. Each I/O operation is written as soon as possible. 
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);
    
    // Associate 'signalHandler' function with interrupt signal (Ctrl+C key)
    signal(SIGINT,signalHandler);

    // Prepare memory for real time performance 
    // https://design.ros2.org/articles/realtime_background.html
    
    // Lock this processe's memory. Necessary for real time performance....
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        printf( "Mlockall failed, check if you have sudo authority.");
        return ;
    }
    /* Turn off malloc trimming.*/
    mallopt(M_TRIM_THRESHOLD, -1);

    /* Turn off mmap usage. */
    mallopt(M_MMAP_MAX, 0);
    // -----------------------------------------------------------------------------
}