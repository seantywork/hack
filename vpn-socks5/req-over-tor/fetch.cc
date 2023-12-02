

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <err.h>

#define SOCKET_ERROR    -1
#define INVALID_SOCKET  -1

typedef int         SOCKET;
typedef sockaddr    SOCKADDR;
typedef sockaddr_in SOCKADDR_IN;

#define closesocket close


#include <iostream>

int main()
{
    std::cout << " [*] C++ Tor" << std::endl;
    std::cout << " [*] Connecting " << std::endl;

    SOCKET      Socket;
    SOCKADDR_IN SocketAddr;

    Socket = socket(AF_INET, SOCK_STREAM, 0);
    SocketAddr.sin_family       = AF_INET;
    SocketAddr.sin_port         = htons(9050);
    SocketAddr.sin_addr.s_addr  = inet_addr("127.0.0.1");

    connect(Socket, (SOCKADDR*)&SocketAddr, sizeof(SOCKADDR_IN));

    char Req1[3] =
    {
        0x05, // SOCKS 5
        0x01, // One Authentication Method
        0x00  // No AUthentication
    };
    send(Socket, Req1, 3, MSG_NOSIGNAL);

    char Resp1[2];
    recv(Socket, Resp1, 2, 0);
    if(Resp1[1] != 0x00)
    {
        std::cout << " [*] Error Authenticating " << std::endl;
        return(-1); // Error
    }

    std::cout << " [*] Fetching... " << std::endl;
    char* Domain = "httpbin.org";
    char  DomainLen = (char)strlen(Domain);
    short Port = htons(80);

    char TmpReq[4] = {
          0x05, // SOCKS5
          0x01, // CONNECT
          0x00, // RESERVED
          0x03, // DOMAIN
        };

    char* Req2 = new char[4 + 1 + DomainLen + 2];

    memcpy(Req2, TmpReq, 4);                // 5, 1, 0, 3
    memcpy(Req2 + 4, &DomainLen, 1);        // Domain Length
    memcpy(Req2 + 5, Domain, DomainLen);    // Domain
    memcpy(Req2 + 5 + DomainLen, &Port, 2); // Port

    send(Socket, (char*)Req2, 4 + 1 + DomainLen + 2, MSG_NOSIGNAL);

    delete[] Req2;

    char Resp2[10];
    recv(Socket, Resp2, 10, 0);
    if(Resp2[1] != 0x00)
    {
      std::cout << " [*] Error : " << Resp2[1] << std::endl;
      return(-1); // ERROR
    }

    std::cout << " [*] Connected " << std::endl;

    std::cout << " [*] Testing with GET Request \n" << std::endl;
    char * request = "GET /ip HTTP/1.1\r\nHost: httpbin.org\r\nCache-Control: no-cache\r\n\r\n\r\n";
    send(Socket, request, strlen(request), MSG_NOSIGNAL);
    char RecvBuffer[2048];
    size_t Rcved = recv(Socket, RecvBuffer, 2048, 0);
    std::cout.write(RecvBuffer, Rcved);

    std::cout << std::endl;

    return(0);
}