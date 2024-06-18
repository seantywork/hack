#ifndef _CAM_WEBM_
#define _CAM_WEBM_

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <errno.h>
#include <stdio.h> 
#include <stdlib.h>

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>

#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavformat/avio.h>
#include <libavformat/avformat.h>

#define PKT_SIZE 1600



extern struct sockaddr_storage addrDest;


int resolvehelper(
  const char* hostname, 
  int family, 
  const char* service, 
  struct sockaddr_storage* pAddr);


int create_sock(char* IP_address, char* port_no);


int sendto_server(int sock, uint8_t* data, int size);


int to_file();


int to_server();

#endif