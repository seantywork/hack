#ifndef _HTTP2_CLIENT_H_
#define _HTTP2_CLIENT_H_

#ifdef __sgi
#  include <string.h>
#  define errx(exitcode, format, args...)                                      \
    {                                                                          \
      warnx(format, ##args);                                                   \
      exit(exitcode);                                                          \
    }
#  define warnx(format, args...) fprintf(stderr, format "\n", ##args)
char *strndup(const char *s, size_t size);
#endif

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>

#include <unistd.h>


#include <sys/socket.h>


#include <netinet/in.h>

#include <netinet/tcp.h>
#ifndef __sgi
#  include <err.h>
#endif
#include <signal.h>
#include <string.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>

#include <event.h>
#include <event2/event.h>
#include <event2/bufferevent_ssl.h>
#include <event2/dns.h>

#define NGHTTP2_NO_SSIZE_T
#include <nghttp2/nghttp2.h>

#include "url-parser/url_parser.h"

#define ARRLEN(x) (sizeof(x) / sizeof(x[0]))

#define MAKE_NV(NAME, VALUE, VALUELEN)                                         \
  {                                                                            \
    (uint8_t *)NAME, (uint8_t *)VALUE, sizeof(NAME) - 1, VALUELEN,             \
        NGHTTP2_NV_FLAG_NONE                                                   \
  }

#define MAKE_NV2(NAME, VALUE)                                                  \
  {                                                                            \
    (uint8_t *)NAME, (uint8_t *)VALUE, sizeof(NAME) - 1, sizeof(VALUE) - 1,    \
        NGHTTP2_NV_FLAG_NONE                                                   \
  }


#ifndef VERIFICATION_LOCATION
# define VERIFICATION_LOCATION "certs/ca.pem"
#endif

typedef struct http2_stream_data{
  /* The NULL-terminated URI string to retrieve. */
  const char *uri;
  /* Parsed result of the |uri| */
  struct http_parser_url *u;
  /* The authority portion of the |uri|, not NULL-terminated */
  char *authority;
  /* The path portion of the |uri|, including query, not
     NULL-terminated */
  char *path;
  /* The length of the |authority| */
  size_t authoritylen;
  /* The length of the |path| */
  size_t pathlen;
  /* The stream ID of this stream */
  int32_t stream_id;
} http2_stream_data;

typedef struct http2_session_data{
  nghttp2_session *session;
  struct evdns_base *dnsbase;
  struct bufferevent *bev;
  http2_stream_data *stream_data;
} http2_session_data;



http2_stream_data *create_http2_stream_data(const char *uri,
                                                   struct http_parser_url *u);


void delete_http2_stream_data(http2_stream_data *stream_data);

http2_session_data *create_http2_session_data(struct event_base *evbase);

void delete_http2_session_data(http2_session_data *session_data);

void print_header(FILE *f, const uint8_t *name, size_t namelen,
                         const uint8_t *value, size_t valuelen);


void print_headers(FILE *f, nghttp2_nv *nva, size_t nvlen);

nghttp2_ssize send_callback(nghttp2_session *session,
                                   const uint8_t *data, size_t length,
                                   int flags, void *user_data);

int on_header_callback(nghttp2_session *session,
                              const nghttp2_frame *frame, const uint8_t *name,
                              size_t namelen, const uint8_t *value,
                              size_t valuelen, uint8_t flags, void *user_data);                                   

int on_begin_headers_callback(nghttp2_session *session,
                                     const nghttp2_frame *frame,
                                     void *user_data);

int on_frame_recv_callback(nghttp2_session *session,
                                  const nghttp2_frame *frame, void *user_data);


int on_data_chunk_recv_callback(nghttp2_session *session, uint8_t flags,
                                       int32_t stream_id, const uint8_t *data,
                                       size_t len, void *user_data);

int on_stream_close_callback(nghttp2_session *session, int32_t stream_id,
                                    uint32_t error_code, void *user_data);


SSL_CTX *create_ssl_ctx(void);

SSL *create_ssl(SSL_CTX *ssl_ctx);

void initialize_nghttp2_session(http2_session_data *session_data);


void send_client_connection_header(http2_session_data *session_data);

void submit_request(http2_session_data *session_data);

int session_send(http2_session_data *session_data);

void readcb(struct bufferevent *bev, void *ptr);

void writecb(struct bufferevent *bev, void *ptr);


void eventcb(struct bufferevent *bev, short events, void *ptr);

void initiate_connection(struct event_base *evbase, SSL_CTX *ssl_ctx,
                                const char *host, uint16_t port,
                                http2_session_data *session_data);

void run(const char *uri);

int verify_callback(int preverify, X509_STORE_CTX* x509_ctx);

void print_cn_name(const char* label, X509_NAME* const name);

void print_san_name(const char* label, X509* const cert);

#endif