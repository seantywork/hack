#ifndef _HTTP2_SERVER_H_
#define _HTTP2_SERVER_H_

#ifdef __sgi
#  define errx(exitcode, format, args...)                                      \
    {                                                                          \
      warnx(format, ##args);                                                   \
      exit(exitcode);                                                          \
    }
#  define warn(format, args...) warnx(format ": %s", ##args, strerror(errno))
#  define warnx(format, args...) fprintf(stderr, format "\n", ##args)
#endif

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <sys/types.h>

#include <sys/socket.h>


#include <netdb.h>

#include <signal.h>

#include <unistd.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <ctype.h>

#include <netinet/in.h>

#include <netinet/tcp.h>
#ifndef __sgi
#  include <err.h>
#endif
#include <string.h>
#include <errno.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/conf.h>

#include <event.h>
#include <event2/event.h>
#include <event2/bufferevent_ssl.h>
#include <event2/listener.h>

#define NGHTTP2_NO_SSIZE_T
#include <nghttp2/nghttp2.h>

#define OUTPUT_WOULDBLOCK_THRESHOLD (1 << 16)

#define ARRLEN(x) (sizeof(x) / sizeof(x[0]))

#define MAKE_NV(NAME, VALUE)                                                   \
  {                                                                            \
    (uint8_t *)NAME, (uint8_t *)VALUE, sizeof(NAME) - 1, sizeof(VALUE) - 1,    \
        NGHTTP2_NV_FLAG_NONE                                                   \
  }


struct app_context;
typedef struct app_context app_context;

typedef struct http2_stream_data {
  struct http2_stream_data *prev, *next;
  char *request_path;
  int32_t stream_id;
  int fd;
} http2_stream_data;

typedef struct http2_session_data {
  struct http2_stream_data root;
  struct bufferevent *bev;
  app_context *app_ctx;
  nghttp2_session *session;
  char *client_addr;
} http2_session_data;

struct app_context {
  SSL_CTX *ssl_ctx;
  struct event_base *evbase;
};


int alpn_select_proto_cb(SSL *ssl, const unsigned char **out,
                                unsigned char *outlen, const unsigned char *in,
                                unsigned int inlen, void *arg);

SSL_CTX *create_ssl_ctx(const char *key_file, const char *cert_file);

SSL *create_ssl(SSL_CTX *ssl_ctx);

void add_stream(http2_session_data *session_data,
                       http2_stream_data *stream_data);

void remove_stream(http2_session_data *session_data,
                          http2_stream_data *stream_data);

http2_stream_data *create_http2_stream_data(http2_session_data *session_data, int32_t stream_id);


void delete_http2_stream_data(http2_stream_data *stream_data);


http2_session_data *create_http2_session_data(app_context *app_ctx,
                                                     int fd,
                                                     struct sockaddr *addr,
                                                     int addrlen);

void delete_http2_session_data(http2_session_data *session_data);



int session_send(http2_session_data *session_data);


int session_recv(http2_session_data *session_data);

nghttp2_ssize send_callback(nghttp2_session *session,
                                   const uint8_t *data, size_t length,
                                   int flags, void *user_data);


int ends_with(const char *s, const char *sub);

uint8_t hex_to_uint(uint8_t c);


char *percent_decode(const uint8_t *value, size_t valuelen);

nghttp2_ssize file_read_callback(nghttp2_session *session,
                                        int32_t stream_id, uint8_t *buf,
                                        size_t length, uint32_t *data_flags,
                                        nghttp2_data_source *source,
                                        void *user_data);

int send_response(nghttp2_session *session, int32_t stream_id,
                         nghttp2_nv *nva, size_t nvlen, int fd);                           


int error_reply(nghttp2_session *session,
                       http2_stream_data *stream_data);


int on_header_callback(nghttp2_session *session,
                              const nghttp2_frame *frame, const uint8_t *name,
                              size_t namelen, const uint8_t *value,
                              size_t valuelen, uint8_t flags, void *user_data);


int on_begin_headers_callback(nghttp2_session *session,
                                     const nghttp2_frame *frame,
                                     void *user_data);

int check_path(const char *path);


int on_request_recv(nghttp2_session *session,
                           http2_session_data *session_data,
                           http2_stream_data *stream_data);

int on_frame_recv_callback(nghttp2_session *session,
                                  const nghttp2_frame *frame, void *user_data);



int on_stream_close_callback(nghttp2_session *session, int32_t stream_id,
                                    uint32_t error_code, void *user_data);


void initialize_nghttp2_session(http2_session_data *session_data);


int send_server_connection_header(http2_session_data *session_data);

void readcb(struct bufferevent *bev, void *ptr);

void writecb(struct bufferevent *bev, void *ptr);


void eventcb(struct bufferevent *bev, short events, void *ptr);

void acceptcb(struct evconnlistener *listener, int fd,
                     struct sockaddr *addr, int addrlen, void *arg);

void start_listen(struct event_base *evbase, const char *service,
                         app_context *app_ctx);


void initialize_app_context(app_context *app_ctx, SSL_CTX *ssl_ctx,
                                   struct event_base *evbase);

void run(const char *service, const char *key_file,
                const char *cert_file);


#endif