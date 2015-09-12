#include "ichttpd.h"

void ichttpd_start(void)
{
    int listenfd, clifd;
    pid_t pid;
    struct ichttpd_conf conf = {"8000", "/var/www"};

    read_conf(CONF_PATH, &conf);

    if ((listenfd = ichttpd_listen(&conf)) < 0)
        exit(EXIT_FAILURE);
    printf(DONE("Listening", "Ready to accept connections\n"));

    for (;;) {
        if ((clifd = ichttpd_accept(listenfd)) < 0)
            continue;

        if ((pid = fork()) < 0) {
            err_sys(WARN("Warning", "fork()"));
            try_close(clifd);
        } else if (pid == 0) {
            /*
             * Child Process
             * Closing socket is handled by ichttpd_response().
             */
            ichttpd_response(clifd, &conf);
            ichttpd_exit(clifd, EXIT_SUCCESS);
        } else {
            /* Parent Process */
            try_close(clifd);
        }
    }
}

/*
 * Create a server endpoint of a connection.
 * Returns fd if success, a negative code indicates error.
 */
int ichttpd_listen(const struct ichttpd_conf *cfg)
{
    int listenfd, opt;
    struct sockaddr_in servaddr;

    /* Create an Internet socket */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_sys(ERROR("Fatal Err", "socket()"));
        return -1;
    }

    /* Fill in socket address structure */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(cfg->port));

    /* In case failed to bind during TIME_WAIT */
    opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Bind the server address to listenfd */
    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))
            < 0) {
        err_sys(ERROR("Fatal Err", "bind()"));
        return -2;
    }

    if (listen(listenfd, BACKLOG) < 0) {
        err_sys(ERROR("Fatal Err", "listen()"));
        return -3;
    }

    return listenfd;
}

int ichttpd_accept(int listenfd)
{
    struct sockaddr_in cliaddr;
    socklen_t cliaddr_len;
    int clifd;

    cliaddr_len = sizeof(cliaddr);
    clifd =
        accept(listenfd, (struct sockaddr *) &cliaddr, &cliaddr_len);
    if (clifd < 0) {
        err_sys(ERROR("Fatal Err", "accept()"));
        return -1;
    }

    return clifd;
}

void ichttpd_response(int connfd, struct ichttpd_conf *cfg)
{
    FILE *fp;                       /* connfd associated stream   */
    //char req_line[MAXLEN];          /* First line of HTTP request */
    struct req_line rqline;           /* The first request line     */
    struct req_header rqheader;       /* Request header fields      */
    //char *method, *url, *version;   /* HTTP head info             */
    char path[MAXLEN];              /* File path of requested URL */
    struct stat filestat;           /* Status of requested file   */

    printf(INFO("New PID", "Child process %d start to response\n"),
            getpid());

    /* Associates fp stream with file descriptor `connfd' */
    if ((fp = fdopen(connfd, "a+")) == NULL) {
        err_sys(WARN("Warning", "fdopen() error"));
        ichttpd_exit(connfd, EXIT_FAILURE);
    }

    read_req_line(fp, &rqline);
    read_req_header(fp, &rqheader);

    /* method = GET or POST */
    if (rqline.rqmethod == M_UNKNOWN) {
        resp_unsupport(fp);
        fprintf(stderr,
                WARN("Method Error", "Unsupported method %s\n"), rqline.method);
        ichttpd_exit(connfd, EXIT_FAILURE);
    }

    /* Easter egg */
    if (strcmp(rqline.url, "/about") == 0) {
        resp_easter(fp);
        ichttpd_exit(connfd, EXIT_SUCCESS);
    }

    /*  /  ->  /index.html  */
    if (strcmp(rqline.url, "/") == 0) {
        full_path(path, MAXLEN, cfg->dir, "/index.htm");
        if (stat(path, &filestat) == 0) {
            rqline.url = "/index.htm";
        } else {
            rqline.url = "/index.html";
        }
    }

    /* Check the existence of requested file */
    full_path(path, MAXLEN, cfg->dir,rqline.url);
    if (stat(path, &filestat) < 0) {
        if (errno == ENOENT) {
            resp_unfound(fp, rqline.url);
            ichttpd_exit(connfd, EXIT_SUCCESS);
        } else
            ichttpd_exit(connfd, EXIT_FAILURE);
    }

    /* Directory */
    if (is_directory(filestat)) {
        resp_directory(fp, path, rqline.url, rqheader.host);
        ichttpd_exit(connfd, EXIT_SUCCESS);
    }

    /* Check if it is executable */
    if (is_executable(filestat) || rqline.querystr != NULL
            || rqline.rqmethod == M_POST) {
        resp_cgi(fp, path, &rqline, &rqheader);
        ichttpd_exit(connfd, EXIT_FAILURE); /* exec doesn't return */
    }

    resp_regfile(fp, path);
    ichttpd_exit(connfd, EXIT_SUCCESS);
}

void ichttpd_exit(int connfd, int exitcode)
{
    printf(INFO("Process Exit", "PID = %d exit\n"), getpid());
    fflush(NULL);       /* Ensure data in streams are sent */
    exit((try_close(connfd) < 0) ? EXIT_FAILURE : exitcode);
}

void read_conf(const char *path, struct ichttpd_conf *cfg)
{
    FILE *fp;
    char line[MAXLEN];

    if ((fp = fopen(path, "r")) == NULL) {
        err_sys(WARN("Warning", "fopen()"));
        printf(WARN("Config", "Using default config instead\n"));
        return;
    }

    while (fgets(line, MAXLEN, fp) != NULL) {
        parse_conf(line, cfg);
    }
    printf(DONE("Read Config", "Port = %s, Directory = %s\n"),
            cfg->port, cfg->dir);

    if (fclose(fp) != 0) {
        err_sys(WARN("Warning", "fclose()"));
    }
}

void parse_conf(char *line, struct ichttpd_conf *cfg)
{
    char *equal;
    char *key, *value;

    if ((equal = strchr(line, '=')) == NULL)
        return;
    *equal = '\0';

    key = strtok(line, SPACES);
    value = strtok(equal + 1, SPACES);

    if (key[0] == '#')
        return;		/* Comment line */

    if (strcmp(key, "Port") == 0)
        strncpy(cfg->port, value, MAXLEN);
    else if (strcmp(key, "Directory") == 0)
        strncpy(cfg->dir, value, MAXLEN);
    else
        fprintf(stderr, WARN("Warning", "Unkown key %s\n"), key);
}

void read_req_line(FILE *sockfp, struct req_line *rqline)
{
    char *saveptr;

    if (fgets(rqline->line, MAXLEN, sockfp) == NULL) {
        err_sys(WARN("Warning", "fgets() returned NULL"));
        ichttpd_exit(fileno(sockfp), EXIT_FAILURE);
    }

    rqline->method = strtok_r(rqline->line, SPACES, &saveptr);
    rqline->url = strtok_r(NULL, SPACES, &saveptr);
    rqline->version = strtok_r(NULL, SPACES, &saveptr);

    if (rqline->method == NULL || rqline->url == NULL
            || rqline->version == NULL) {
        fprintf(stderr,
                WARN("Warning", "Invalid HTTP head format\n"));
        ichttpd_exit(fileno(sockfp), EXIT_FAILURE);
    }

    if ((rqline->querystr = strchr(rqline->url, '?')) != NULL)
        *rqline->querystr++ = '\0';

    if (strcasecmp(rqline->method, "GET") == 0)
        rqline->rqmethod = M_GET;
    else if (strcasecmp(rqline->method, "POST") == 0)
        rqline->rqmethod = M_POST;
    else
        rqline->rqmethod = M_UNKNOWN;
}

void read_req_header(FILE *sockfp, struct req_header *rqheader)
{
    char buf[MAXLEN];
    char *key, *val, *saveptr;
    rqheader->content_len = -1;

    memset(rqheader->host, '\0', MAXLEN);
    rqheader->content_len = 0;

    while (fgets(buf, MAXLEN, sockfp) != NULL) {
        /*printf("%s", buf); */
        if (strcmp(buf, "\r\n") == 0)
            break;

        key = strtok_r(buf, SPACES, &saveptr);
        val = strtok_r(NULL, NEWLINES, &saveptr);

        if (strcasecmp(key, "Host:") == 0)
            strncpy(rqheader->host, val, MAXLEN);
        else if (strcasecmp(key, "Content-Length:") == 0)
            rqheader->content_len = atoi(val);
    }

    /*puts(host); */
}

void resp_easter(FILE * sockfp)
{
    write_head(sockfp, 200);
    write_filetype(sockfp, "html");

    html_start(sockfp, "html");
    html_title(sockfp, "About");
    html_start(sockfp, "body");

    html_header(sockfp, 1, "Coded by Wang Hsutung");
    try_fprintf(sockfp, "<p>A simple HTTP server `<b>ICHttpd</b>'</p>"
            "<i>ICH <del>LIEBE</del> DICH</i><h2>;-)</h2>");
    html_link(sockfp, "王旭东", "http://qzone.qq.com/757224305");
    try_fprintf(sockfp, ", Aug 2015, <a href = \"mailto:"
            "xukiro@outlook.com\">xukiro@outlook.com</a>");

    html_end(sockfp, "body");
    html_end(sockfp, "html");
}

void resp_unsupport(FILE * sockfp)
{
    write_head(sockfp, 200);
    write_filetype(sockfp, "html");

    html_page(sockfp, "Unsupported", "Unsupported method",
            "<p>Methods other than GET are not implemented yet.</p>"
            "Please wait.<h2>:-)</h2>");
}

void resp_unfound(FILE * sockfp, const char *url)
{
    fprintf(stderr, WARN("Error 404", "Cannot find file `%s'\n"), url);
    write_head(sockfp, 404);
    write_filetype(sockfp, "html");

    html_page(sockfp, "Error 404", "404 Not Found",
            "Oops, file <b>%s</b> can't be found.<h2>:-(</h2>", url);
}

void resp_directory(FILE * sockfp, const char *path, const char *url,
        const char *host)
{
    char buf[MAXLEN];
    size_t len;
    DIR *dirp;
    struct dirent *dp;

    if ((dirp = opendir(path)) == NULL) {
        err_sys(ERROR("File Error", "opendir %s"), path);
        return;
    }

    write_head(sockfp, 200);
    write_filetype(sockfp, "html");

    html_start(sockfp, "html");
    html_title(sockfp, path);
    html_start(sockfp, "body");

    snprintf(buf, MAXLEN, "<i>%s</i> is a directory", url);
    html_header(sockfp, 1, buf);

    while ((dp = readdir(dirp)) != NULL) {
        if (strcmp(dp->d_name, ".") == 0)
            continue;	/* Current folder */
        snprintf(buf, MAXLEN - 1, "http://%s%s", host, url);
        /* Add a slash to the end if there isn't one */
        len = strlen(buf);
        if (buf[len - 1] != '/') {
            buf[len] = '/';
            buf[++len] = '\0';
        }
        /* Let buf be the full URL of this file */
        strncpy(buf + len, dp->d_name, MAXLEN - len);
        /* Print a link of this file on HTML page */
        html_start(sockfp, "p");
        html_link(sockfp, dp->d_name, buf);
        html_end(sockfp, "p");
    }

    html_end(sockfp, "body");
    html_end(sockfp, "html");
}

void resp_cgi(FILE *sockfp, const char *path,
        struct req_line *rqline, struct req_header *rqheader)
{
    pid_t   pid;
    int     status;
    int     cgi_input[2], cgi_output[2];
    //char    buf[MAXLEN];
    char    c;
    char    querystr_env[MAXLEN];
    char    contentlen_env[MAXLEN];
    char    method_env[MAXLEN];

    /* Create pipe */
    if (pipe(cgi_input) < 0 || pipe(cgi_output) < 0) {
        err_sys(ERROR("pipe Error", "pipe() for CGI"));
        ichttpd_exit(fileno(sockfp), EXIT_FAILURE);
    }

    if ((pid = fork()) < 0) {
        err_sys(WARN("Warning", "fork() for CGI"));
        ichttpd_exit(fileno(sockfp), EXIT_FAILURE);

    } else if (pid == 0) {
        /* Child Process */
        snprintf(method_env, MAXLEN, "REQUEST_METHOD=%s", rqline->method);
        putenv(method_env);

        if (rqline->rqmethod == M_GET) {
            snprintf(querystr_env, MAXLEN, "QUERY_STRING=%s", rqline->querystr);
            putenv(querystr_env);
        } else if (rqline->rqmethod == M_POST) {
            snprintf(contentlen_env, MAXLEN, "CONTENT_LENGTH=%d", rqheader->content_len);
            putenv(contentlen_env);
        }

        /* Duplicate file descriptor */
        if (dup2(cgi_input[0], STDIN_FILENO) < 0 ||
                dup2(cgi_output[1], STDOUT_FILENO) < 0) {
            err_sys(ERROR("dup2 Error", "dup2() for CGI"));
            return;
        }
        if (try_close(cgi_input[1]) < 0 || try_close(cgi_output[0]) < 0)
            return;

        write_head(stdout, 200);
        /* Execute file file */
        execl(path, path, NULL);
        err_sys(ERROR("execl Error", "execl() for CGI"));

    } else {
        /* Parent Process */
        if (try_close(cgi_input[0]) < 0 || try_close(cgi_output[1]) < 0)
            return;
        if (rqline->rqmethod == M_POST) {
            int i;
            printf("把post数据给子进程 %d\n", rqheader->content_len);//DEBUG
            for (i = 0; i < rqheader->content_len; ++i) {
                c = getc(sockfp);
                write(cgi_input[1], &c, 1);
                printf("post 1 byte %c\n", c);
            }
        }

        while (read(cgi_output[0], &c, 1) > 0)
            putc(c, sockfp);

        if (try_close(cgi_input[1]) < 0 || try_close(cgi_output[0]) < 0)
            return;

        waitpid(pid, &status, 0);
        printf("子进程结束了\n");
    }
}

void resp_regfile(FILE * sockfp, const char *path)
{
    FILE *filefp;
    int c;  /* `char  c' will NOT work */

    if ((filefp = fopen(path, "rb")) == NULL) {
        err_sys(ERROR("File Error", "fopen() `%s'"), path);
        return;
    }

    write_head(sockfp, 200);
    write_filetype(sockfp, path);

    /* Read the file and send */
    while ((c = getc(filefp)) != EOF)
        putc(c, sockfp);

    fclose(filefp);
    printf(DONE("File Sent", "%s sent to client successfully\n"),
            path);
}

void write_head(FILE * fp, int code)
{
    const char *msg;

    switch (code) {
    case 200:
        msg = "OK";
        break;
    case 404:
        msg = "Not Found";
        break;
    default:
        msg = "OK";
    }
    if (fprintf(fp, "HTTP/1.1 %d %s\r\n", code, msg) < 0) {
        err_sys(WARN("Warning", "Failed to response http head"));
    }
}

void write_filetype(FILE * fp, const char *path)
{
    const char *type, *ext;

    ext = get_extname(path);

    if (strcasecmp(ext, "html") == 0 || strcasecmp(ext, "htm") == 0)
        type = "text/html";
    else if (strcasecmp(ext, "jpg") == 0
            || strcasecmp(ext, "jpeg") == 0)
        type = "image/jpeg";
    else if (strcasecmp(ext, "png") == 0)
        type = "image/png";
    else if (strcasecmp(ext, "ico") == 0)
        type = "image/ico";
    else if (strcasecmp(ext, "txt") == 0)
        type = "text/plain";
    else if (strcasecmp(ext, "css") == 0)
        type = "text/css";
    else {
        fprintf(stderr,
                WARN("Unknown Type", "Regard `%s' as text\n"),
                path);
        type = "text/plain";
    }

    printf(INFO("Resp Type", "File type: %s\n"), type);
    if (fprintf(fp, "Content-Type: %s\r\n\r\n", type) < 0) {
        err_sys(WARN("Warning", "Failed to response MIME type"));
        return;
    }
}
