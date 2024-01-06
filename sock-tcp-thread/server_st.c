
#include "server_st.h"

   
int main() 
{ 

    int main_thread = 10;





    for (int i = 0; i < MAX_CONN; i ++){

        CLIENT_SOCKET[i] = 0;
    }

    SOCKFD = socket(AF_INET, SOCK_STREAM, 0); 
    if (SOCKFD == -1) { 
        printf("socket creation failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    else
        printf("socket successfully created\n"); 

    if( setsockopt(SOCKFD, SOL_SOCKET, SO_REUSEADDR, (char *)&OPT,  
          sizeof(OPT)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     

    bzero(&SERVADDR, sizeof(SERVADDR)); 
   
    SERVADDR.sin_family = AF_INET; 
    SERVADDR.sin_addr.s_addr = htonl(INADDR_ANY); 
    SERVADDR.sin_port = htons(PORT); 
   
    if ((bind(SOCKFD, (SA*)&SERVADDR, sizeof(SERVADDR))) != 0) { 
        printf("socket bind failed\n"); 
        exit(EXIT_FAILURE); 
    } 
   

    if ((listen(SOCKFD, 5)) != 0) { 
        printf("listen failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    else{
        SERVLEN = sizeof(SERVADDR); 
    }



    sem_init(&TLOCK, 0, 1);

    for (int i = 0; i < MAX_WORKER; i++){

        int err = pthread_create(&(TID[i]), NULL, Worker, (void *)&(i));

        if (err != 0){
            printf("error creating thread\n");
            return EXIT_FAILURE;
        }
    }




    Worker((void *)&main_thread);


    return EXIT_SUCCESS;
}