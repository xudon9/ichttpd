/***************************\
 * Author: Wang Hsutung
 * Date: 2015/08/11
 * Locale: 家里
 * Email: hsu[AT]whu.edu.cn
 * Last Update: 2015/09/13
\***************************/

#ifndef ICHTTPD_H
#define ICHTTPD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include "utils.h"

/** Config file path **/
#define CONF_PATH    "./ichttpd.conf"

#define MAXLEN  (1 << (1 * 8))
#define BACKLOG 10
#define SPACES " \f\n\r\t\v"
#define NEWLINES "\n\r"

enum req_method
{
    M_UNKNOWN, M_GET, M_POST
};

struct ichttpd_conf {
    char    port[MAXLEN];
    char    dir[MAXLEN];
};

struct req_line {
    char    line[MAXLEN];
    char    *method, *url, *version;
    char    *querystr;
    enum req_method mthd_id;
};

struct req_header {
    char    host[MAXLEN];
    int     contentlen;
};

void ichttpd_start   (void);
int  ichttpd_listen  (const struct ichttpd_conf *cfg);
int  ichttpd_accept  (int listenfd);
void ichttpd_response(int connfd, struct ichttpd_conf *cfg);
void ichttpd_exit    (int connfd, int exitcode);

void read_conf       (const char *path, struct ichttpd_conf *cfg);
void parse_conf      (char *line, struct ichttpd_conf *cfg);

void read_req_line   (FILE *sockfp, struct req_line *line);
void read_req_header (FILE *sockfp, struct req_header *header);

void resp_easter     (FILE *sockfp);
void resp_unsupport  (FILE *sockfp);
void resp_unfound    (FILE *sockfp, const char *url);
void resp_directory  (FILE *sockfp, const char *path, const char *url, const char *host);
void resp_regfile    (FILE *sockfp, const char *path);
void resp_cgi        (FILE *sockfp, const char *path,
        const struct req_line *line, const struct req_header *header);

void write_head      (FILE *fp, int code);
void write_filetype  (FILE *fp, const char *path);

#endif /* ICHTTPD_H */

