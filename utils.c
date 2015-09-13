/***************************\
 * Author: Wang Hsutung
 * Date: 2015/08/12
 * Locale: 家里
 * Email: hsu[AT]whu.edu.cn
\***************************/

#include "utils.h"

void consume_rest(FILE *fp)
{
    char    buf[BUFSIZE];
    while (fgets(buf, BUFSIZE, fp) != NULL) {
        if (strcmp(buf, "\r\n") == 0)
            break;
        /*printf("-> %s", buf);*/
    }
}

void try_fprintf(FILE *fp, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    if (vfprintf(fp, fmt, ap) < 0)
        err_sys(WARN("Warn", "vfprintf() at line:%d file:%s"),
                __LINE__, __FILE__);
    va_end(ap);
}

void err_sys(const char *fmt, ...)
{
    int err = errno;
    char buf[BUFSIZE + 1];
    va_list ap;

    va_start(ap, fmt);

    vsnprintf(buf, BUFSIZE, fmt, ap);
    snprintf(buf+strlen(buf), BUFSIZE-strlen(buf), ": %s", strerror(err));
    strcat(buf, "\n");
    fputs(buf, stderr);

    va_end(ap);
}

int try_close(int fd)
{
    int val;
    while ((val = close(fd)) < 0) {
        if (errno == EINTR)
            continue;
        err_sys(WARN("Warn", "close() at line:%d file:%s"), __LINE__, __FILE__);
        break;
    }
    return val;
}

const char *get_extname(const char *filename)
{
    char *dot;
    if ((dot = strrchr(filename, '.')) == NULL)
        return filename;
    return dot + 1;
}

/* path = dir/file, file should start with slash */
void full_path(char *path, size_t n, const char *dir, const char *file)
{
    size_t dir_len = strlen(dir);

    strncpy(path, dir, n);
    if (dir[dir_len - 1] == '/')
        path[--dir_len] = '\0';
    strncpy(path + dir_len, file, n - dir_len);
}

/* HTML tags generators */
void html_page(FILE *fp, const char *title,
        const char *head, const char *text, ...)
{
    va_list ap;

    html_start(fp, "html");
    html_title(fp, title);
    html_start(fp, "body");
    html_header(fp, 1, head);

    va_start(ap, text);
    if (vfprintf(fp, text, ap) < 0)
        err_sys(WARN("Warn", "vfprintf error"));
    va_end(ap);

    html_end(fp, "body");
    html_end(fp, "html");
}

void html_header(FILE *fp, int level, const char *msg)
{
    if (level > 6)      level = 6;
    else if (level < 1) level = 1;
    try_fprintf(fp, "<h%d>%s</h%d>\r\n", level, msg, level);
}

void html_link(FILE *fp, const char *msg, const char *url)
{
    try_fprintf(fp, "<a href=\"%s\" title=\"%s\">%s</a>", url, msg, msg);
}

void html_title (FILE *fp, const char *msg)
{
    try_fprintf(fp, "<head><meta charset=\"UTF-8\"><title>%s</title></head>\r\n", msg);
}
void html_start (FILE *fp, const char *tag) { try_fprintf(fp, "<%s>", tag); }
void html_end   (FILE *fp, const char *tag) { try_fprintf(fp, "</%s>\r\n", tag); }

