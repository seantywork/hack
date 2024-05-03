#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ws.h>


void onopen(ws_cli_conn_t *client)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("Connection opened, addr: %s\n", cli);
}


void onclose(ws_cli_conn_t *client)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("Connection closed, addr: %s\n", cli);
}


void onmessage(ws_cli_conn_t *client,
    const unsigned char *msg, uint64_t size, int type)
{
    char *cli;
    cli = ws_getaddress(client);
    printf("I receive a message: %s (%zu), from: %s\n", msg,
        size, cli);

    sleep(2);
    ws_sendframe_txt(client, "hello");
    sleep(2);
    ws_sendframe_txt(client, "world");
}

int main(void)
{

    struct ws_events evs;
    evs.onopen    = &onopen;
    evs.onclose   = &onclose;
    evs.onmessage = &onmessage;

    ws_socket(&evs, 8080, 0, 1000);

    return (0);
}