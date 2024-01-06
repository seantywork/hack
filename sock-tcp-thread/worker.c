
#include "server_st.h"



void* Worker(void* vargp){

    FILE *fp;


    int sd;
    int activity;
    int new_socket;
    int socke_added = FALSE;

    fd_set READFDS;

    char str[10] = {0};


    int *wtid = (int *)vargp; 


    sprintf(str, "%d", *wtid);

    fp = fopen(str, "a");

    fprintf(fp, "writing to thread \n");
    fflush(fp);

    while(TRUE){

        fprintf(fp, "1 \n");
        fflush(fp);

        int rsec = msleep(10);

        FD_ZERO(&READFDS);
        // LOCK!!

        fprintf(fp, "2 \n");
        fflush(fp);
        //sem_wait(&TLOCK);

        FD_SET(SOCKFD, &READFDS);

        MAX_SD = SOCKFD;

        for (int i= 0 ; i < MAX_CONN; i ++){

            sd = CLIENT_SOCKET[i];

            if(sd > 0){
                FD_SET(sd, &READFDS);
            }

            if (sd > MAX_SD){
                MAX_SD = sd;
            }


        }

        fprintf(fp, "%d \n", MAX_SD);
        fflush(fp);

        activity = select(MAX_SD + 1, &READFDS, NULL, NULL, NULL);

        fprintf(fp, "4 \n");
        fflush(fp);

        if ((activity < 0 ) && (errno != EINTR)){
            fprintf(fp,"select error\n");
            fflush(fp);
        }

        fprintf(fp, "check \n");
        fflush(fp);

        if (FD_ISSET(SOCKFD, &READFDS)){

            fprintf(fp,"new conn at thread\n");
            fflush(fp);

            new_socket = accept(SOCKFD, (SA* )&SERVADDR, (socklen_t*)&SERVLEN);

            if (new_socket < 0){
        
                fprintf(fp,"accept error\n");
                fflush(fp);
                exit(EXIT_FAILURE);
            }

            

            for (int i = 0 ; i < MAX_CONN; i ++){

                if(CLIENT_SOCKET[i] == 0){
                    CLIENT_SOCKET[i] = new_socket;
                    fprintf(fp,"added new conn at\n");
                    fflush(fp);
                    socke_added = TRUE;
                    break;
                }
            }

            if(socke_added != TRUE){
                fprintf(fp,"connection max reached\n");
                fflush(fp);
                if(close(new_socket)){
                    fprintf(fp,"refusing incoming conn\n");
                    fflush(fp);
                }else{
                    fprintf(fp,"error closing conn\n");
                    fflush(fp);
                    exit(EXIT_FAILURE);
                }
            } else {
                socke_added = FALSE;
            }

        }
        
        // UNLOCK!!

        //sem_post(&TLOCK);

        Comm(fp);

    }

    flclose(fp);
}

void Comm(FILE* fp, fd_set READFDS) { 
    int sd;
    int valread;
    char buff[MAX_BUFF]; 
    struct sockaddr_in peeraddr;

    for (int i = 0; i < MAX_CONN; i ++) { 

        bzero(buff, MAX_BUFF); 

        sd = CLIENT_SOCKET[i];

        if(FD_ISSET(sd, &READFDS)){

            valread = read(sd, buff, MAX_BUFF);
            if(valread == 0){

                getpeername(sd, (SA*)&peeraddr, (socklen_t*)&SERVLEN);
                fprintf(fp,"client disconnected\n");
                fflush(fp);
                close(sd);

                // LOCK!!!
                sem_wait(&TLOCK);
                CLIENT_SOCKET[i] = 0;

                // UNLOCK!!
                sem_post(&TLOCK);
                
            }else{
                buff[valread] = '\0';
                send(sd, buff, strlen(buff), 0);
            }

        }


    } 
} 


int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}