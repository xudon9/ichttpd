/***************************\
 * Author: Wang Hsutung
 * Date: 2015/08/12
 * Locale: 家里
 * Email: hsu@whu.edu.cn
\***************************/

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define RED(_str)    "\033[1;31m" _str "\033[m"
#define GREEN(_str)  "\033[1;32m" _str "\033[m"
#define YELLOW(_str) "\033[1;33m" _str "\033[m"
#define BLUE(_str)   "\033[1;34m" _str "\033[m"

#define DONE(_title, _msg)  GREEN("["_title"]")"\t"_msg
#define INFO(_title, _msg)  BLUE("["_title"]")"\t"_msg
#define WARN(_title, _msg)  YELLOW("["_title"]")"\t"_msg
#define ERROR(_title, _msg) RED("["_title"]")"\t"_msg

#define is_directory(_fstat)    (S_ISDIR((_fstat).st_mode))
#define is_executable(_fstat)   ((_fstat).st_mode & S_IXUSR ||\
        (_fstat).st_mode & S_IXGRP ||\
        (_fstat).st_mode & S_IXOTH)



#define BUFSIZE (1 << (1 * 8))

extern void     consume_rest(FILE *fp);
extern void     err_sys     (const char *fmt, ...);
extern void     try_fprintf (FILE *fd, const char *fmt, ...);
extern int      try_close   (int fd);
extern void     full_path   (char *path, size_t n, const char *dir, const char *file);
extern const char *get_extname(const char *filename);

/* HTML tags generate */
extern void     html_page   (FILE *fp, const char *title, const char *head, const char *text, ...);
extern void     html_header (FILE *fd, int level, const char *msg);
extern void     html_link   (FILE *fd, const char *msg, const char *url);
extern void     html_title  (FILE *fd, const char *msg);
extern void     html_start  (FILE *fd, const char *tag);
extern void     html_end    (FILE *fd, const char *tag);

#endif /* UTILS_H */

