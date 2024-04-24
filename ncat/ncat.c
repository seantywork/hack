#include "ncat.h"



void NCAT_keyboard_interrupt(){

    fprintf(stderr,"SIGINT. EXIT.\n");

    exit(0);

}


int NCAT_parse_args(int argc, char** argv){


    if(argc < 2){

        return -1;
    }


    int argc_nohp = argc - 2;


    for(int i = 0 ; i < argc_nohp; i ++){


        if(
            (strcmp(argv[i], "--listen") == 0)
            || (strcmp(argv[i], "-l") == 0)
        ){

            NCATOPTS.mode_listen = 1;

        }


    }

    if(NCATOPTS.mode_listen == 0){
        NCATOPTS.mode_client = 1;
    }


    int host_idx = argc_nohp;
    int port_idx = argc_nohp + 1;


    NCATOPTS.host = argv[host_idx];
    NCATOPTS.port = argv[port_idx];




    return 0;
}



void NCAT_free(){



    for(int i = 0 ; i < ncat_argc; i ++){


        free(ncat_argv[i]);


    }

    free(ncat_argv);






}


int NCAT_runner(){

    int status = 0;

    pthread_t thread_id;


    pthread_create(&thread_id, NULL, NCAT_get_thread, NULL);


    if(NCATOPTS.mode_client == 1){

        ncat_connect = 1;

        status = NCAT_client();


        return status;


    }




    if(NCATOPTS.mode_listen == 1){

        ncat_listen = 1;

        status = NCAT_listen_and_serve();


        return status;

    } 


    fprintf(stderr, "unsupported mode\n");

    return -10;

}



int NCAT_client(){


    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    struct in_addr ip_addr;

    in_addr_t s_addr = inet_addr(NCATOPTS.host);

    int addr_port = atoi(NCATOPTS.port);


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        fprintf(stderr, "socket creation failed\n");
        return -1;
    }
 
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = s_addr;
    servaddr.sin_port = htons(addr_port);

    while(ncat_connect){

        if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))!= 0) {
            fprintf(stderr, "connection failed\n");
            return -1;
        }

        pthread_mutex_lock(&NCATOPTS._lock);

        NCATOPTS._client_sockfd = sockfd;
        NCATOPTS._client_sock_ready = 1;

        pthread_mutex_unlock(&NCATOPTS._lock);

        int keepalive = 1;


        while(keepalive){

            char wbuff[MAX_WRITE_BUFF] = {0};

            int valread = 0;

            fgets(wbuff, MAX_WRITE_BUFF, stdin);

            for(int i = MAX_WRITE_BUFF - 1; i > 0; i--){

                if(wbuff[i] == '\n'){
                    wbuff[i] = '\0';
                    break;
                }

            }

            pthread_mutex_lock(&NCATOPTS._lock);

          
            if(NCATOPTS._client_sock_ready != 1){



                pthread_mutex_unlock(&NCATOPTS._lock);
                break;
            }

            pthread_mutex_unlock(&NCATOPTS._lock);
            
            write(sockfd, wbuff, MAX_WRITE_BUFF * sizeof(char));
            

        }   

        pthread_mutex_lock(&NCATOPTS._lock);

        NCATOPTS._client_sock_ready = 0;

        pthread_mutex_unlock(&NCATOPTS._lock);

        close(sockfd);

    }

 
    return 0;
}


int NCAT_listen_and_serve(){

    
    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
    struct in_addr ip_addr;

    in_addr_t s_addr = inet_addr(NCATOPTS.host);
    
    int addr_port = atoi(NCATOPTS.port);

    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        fprintf(stderr,"socket creation failed...\n"); 
        return -1;
    } 

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = s_addr; 
    servaddr.sin_port = htons(addr_port); 
   
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) { 
        fprintf(stderr, "socket bind failed\n"); 
        return -1; 
    } 
   
    if ((listen(sockfd, 5)) != 0) { 
        fprintf(stderr,"socket listen failed\n"); 
        return -1; 
    } 
    
    
    int clilen = sizeof(cli); 


    while(ncat_listen){


        connfd = accept(sockfd, (struct sockaddr*)&cli, (socklen_t*)&clilen); 
        if (connfd < 0) { 
            fprintf(stderr, "server accept failed\n"); 
            return -1; 
        } 
    

        int valwrite =0;

        if(serve_content_exists == 1){

            valwrite = write(connfd, serve_content, MAX_LOAD_BUFF * sizeof(char));

            if(valwrite <= 0){

                fprintf(stderr,"write: %d\n", valwrite);

            }
        }

        int keepalive = 1;

        while(keepalive){

            char rbuff[MAX_READ_BUFF] = {0};

            int valread = 0;

            while(valread < MAX_READ_BUFF){

                char tmp[MAX_READ_BUFF] = {0};

                int rb = read(connfd, tmp, MAX_READ_BUFF);

                if (rb == 0){

                    keepalive = 0;
                    break;

                } 

                if (rb < 0){
                    fprintf(stderr,"error reading");
                    keepalive = 0;
                    break;
                }

                for(int i = 0 ; i < rb; i++){

                    int idx = valread + i;

                    rbuff[idx] = tmp[i];

                }

                valread += rb;

            }

            if(!keepalive){
                continue;
            }

            fprintf(stdout, "%s\n", rbuff);

        }


        close(connfd);



    }

    close(sockfd);



    return 0;
}



void* NCAT_get_thread(){

    if(NCATOPTS.mode_client){

        for(;;){

            pthread_mutex_lock(&NCATOPTS._lock);

            if(NCATOPTS._client_sock_ready){

                int valread = 0;

                char rbuff[MAX_READ_BUFF] = {0};

                int _exit=0;
                
                while(valread < MAX_READ_BUFF){
                    
                    char tmp[MAX_READ_BUFF] = {0};

                    int rb = read(NCATOPTS._client_sockfd, tmp, MAX_READ_BUFF * sizeof(char));

                    if(rb <= 0){
                        
                        _exit = 1;

                        break;

                    }

                    for(int i = 0; i < rb; i++){

                        int idx = valread + i;

                        rbuff[idx] = tmp[i];

                    }
                    
                    valread += rb;

                }
               
                if(_exit == 1){

                    NCATOPTS._client_sock_ready = 0;

                    pthread_mutex_unlock(&NCATOPTS._lock);

                    continue;
                }

                fprintf(stdout, "%s \n", rbuff);
                
                pthread_mutex_unlock(&NCATOPTS._lock);

            } else {
                
                pthread_mutex_unlock(&NCATOPTS._lock);
                msleep(500);

            }


        }


    } else if (NCATOPTS.mode_listen){

        serve_content_exists = 1;

        int line_idx = 0;

        //fgets(serve_content, MAX_LOAD_BUFF, stdin);

        //line_idx = strlen(serve_content);

        char tmp[MAX_TMP_BUFF] = {0};

        for(int i = 0 ; i < 10; i ++){

            memset(tmp, 0, MAX_TMP_BUFF * sizeof(char));

            fgets(tmp, MAX_TMP_BUFF, stdin);

            int len_count = strlen(tmp);

            if(serve_content_ptr + len_count > MAX_LOAD_BUFF){
                break;
            }

            for(int i = 0 ; i < len_count; i++){

                int idx = serve_content_ptr + i;

                serve_content[idx] = tmp[i];

            }

            serve_content_ptr += len_count;


        }
        
        


    }

}


void msleep(long ms){

    struct timespec ts;
    int res;

    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;

    nanosleep(&ts, &ts);
}