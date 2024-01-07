
#include "server_st.h"



void* Worker(void* vargp){

    int alive = TRUE;

    FILE *fp;

    int sd;
    int valread;
    char buff[MAX_BUFF]; 
    struct sockaddr_in peeraddr;

    char f_name[10] = {0};

    struct WorkerArg* warg = (struct WorkerArg* )vargp;

    int wk_tid = warg->WTHREAD_ID;

    sprintf(f_name,"%d", wk_tid);

    fp = fopen(f_name, "a");
    
    sd = warg->WSOCK_FD;

    while(alive){

        bzero(buff, MAX_BUFF); 

        valread = read(sd, buff, MAX_BUFF);
        if(valread == 0){

            getpeername(sd, (SA*)&peeraddr, (socklen_t*)&SERVLEN);
            fprintf(fp,"client disconnected\n");
            fflush(fp);
            close(sd);

            // LOCK!!!
            pthread_mutex_lock(&TLOCK);
            CLIENT_SOCKET[wk_tid] = 0;

            // UNLOCK!!
            pthread_mutex_unlock(&TLOCK);
            
        }else{
            buff[valread] = '\0';
            fprintf(fp, buff);
            fflush(fp);
            send(sd, buff, strlen(buff), 0);
        }



    }

    fclose(fp);


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