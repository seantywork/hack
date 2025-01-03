#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "chan.h"

chan_t* messages;
chan_t* signals;

void* ping()
{

    sleep(3);
    // Send blocks until receiver is ready.
    chan_send(messages, "ping");
    return NULL;
}

int main()
{
    // Initialize channels.
    messages = chan_init(0);
    signals = chan_init(0);
    void *msg;

    // This is a non-blocking receive. If a value is available on messages,
    // select will take the messages (0) case with that value. If not, it will
    // immediately take the default case.

    pthread_t th;
    pthread_create(&th, NULL, ping, NULL);

    int done = 0;

    for(;;){

        
        switch(chan_select(&messages, 1, &msg, NULL, 0, NULL))
        {
            case 0:
                printf("received message %s\n", (char *)msg);
                done = 1;
                break;
            default:
                printf("no message received\n");
        }

        if(done == 1){
            break;
        }
        sleep(1);
    }


    // A non-blocking send works similarly.
    /*
    msg = "foo";
    switch(chan_select(NULL, 0, NULL, &messages, 1, &msg))
    {
        case 0:
            printf("sent message %s\n", (char *)msg);
            break;
        default:
            printf("no message sent\n");
    }
    */

    // We can use multiple cases above the default clause to implement a
    // multi-way non-blocking select. Here we attempt non-blocking receives on
    // both messages and signals.
    /*
    chan_t* chans[2] = {messages, signals};
    switch(chan_select(chans, 2, &msg, NULL, 0, NULL))
    {
        case 0:
            printf("received message %s\n", (char *)msg);
            break;
        case 1:
            printf("received signal %s\n", (char *)msg);
            break;
        default:
            printf("no activity\n");
    }
    */

    // Clean up channels.
    chan_dispose(messages);
    chan_dispose(signals);
}