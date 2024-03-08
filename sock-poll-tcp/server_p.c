#include <server_p.h>


int create_socket()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bindResult = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (bindResult == -1)
    {
        perror("bindResult");
    }

    int listenResult = listen(server_socket, 5);
    if (listenResult == -1)
    {
        perror("listenResult");
    }
    printf("server start\n");
    return server_socket;
}

int wait_client(int server_socket)
{
    struct pollfd pollfds[MAX_CLIENTS + 1];
    pollfds[0].fd = server_socket;
    pollfds[0].events = POLLIN | POLLPRI;

    for (int i = 1; i < MAX_CLIENTS + 1; i++)
    {

        pollfds[i].fd = 0;
        pollfds[i].events = POLLIN | POLLPRI;


    }

    int useClient = 0;

    while (1)
    {
        // printf("useClient => %d\n", useClient);
        int pollResult = poll(pollfds, MAX_CLIENTS + 1, 5000);
        printf("poll: %d\n", pollResult);
        if (pollResult > 0)
        {
            if (pollfds[0].revents & POLLIN)
            {
                struct sockaddr_in cliaddr;
                int addrlen = sizeof(cliaddr);
                int client_socket = accept(server_socket, (struct sockaddr *)&cliaddr, (socklen_t*)&addrlen);
                printf("accept success %s\n", inet_ntoa(cliaddr.sin_addr));
                for (int i = 1; i < MAX_CLIENTS + 1; i++)
                {
                    if (pollfds[i].fd == 0)
                    {

                        pollfds[i].fd = client_socket;
                        useClient++;
                        break;
                    }
                }
            }
            for (int i = 1; i < MAX_CLIENTS + 1; i++)
            {
                if (pollfds[i].fd > 0 && pollfds[i].revents & POLLIN)
                {
                    char buf[SIZE];
                    int bufSize = read(pollfds[i].fd, buf, SIZE - 1);
                    if (bufSize == -1)
                    {
                        pollfds[i].fd = 0;
                        pollfds[i].events = 0;
                        pollfds[i].revents = 0;
                        useClient--;
                        printf("client error\n");
                    }
                    else if (bufSize == 0)
                    {
                        pollfds[i].fd = 0;
                        pollfds[i].events = 0;
                        pollfds[i].revents = 0;
                        useClient--;

                        printf("client eof\n");
                    }
                    else
                    {

                        buf[bufSize] = '\0';
                        printf("From client: %s\n", buf);
                    }
                } 
            }
        }
    }
}

int main()
{
    int server_socket = create_socket();

    int client_socket = wait_client(server_socket);

    printf("server end\n");

    close(client_socket);
    close(server_socket);

    return 0;
}