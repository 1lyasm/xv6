#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

int matchhere(char*, char*);
int matchstar(int, char*, char*);

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}


char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}


int find(char *dir, char *regex) {
  /* printf("find: dir: %s, regex: %s\n", dir, regex); */
  int retcode = 0;
  char buf[512];
  int fd;
  char *p;
  struct dirent de;
  struct stat st;
  if ((fd = open(dir, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", dir);
    retcode = 1;
  } else {
    if (fstat(fd, &st) < 0) {
      fprintf(2, "find: cannot stat %s\n", dir);
      retcode = 1;
    } else {
      switch (st.type) {
        case T_DIR:
          if (strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf) {
            fprintf(2, "find: path too long\n");
            retcode = 1;
          } else {
            strcpy(buf, dir);
            p = buf + strlen(buf);
            *p++ = '/';
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
              if (de.inum == 0) {
                continue;
              }
              memmove(p, de.name, DIRSIZ);
              p[DIRSIZ] = 0;
              if (stat(buf, &st) < 0) {
                fprintf(2, "find: cannot stat %s\n", buf);
                continue;
              }
              /* printf("buf: %s, p: %s, regex: %s\n", buf, p, regex); */
              if (match(regex, p) == 1) {
                fprintf(1, "%s\n", buf);
              }
              if (strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
                int buflen = strlen(buf);
                char *bufcp = malloc((buflen + 1) * sizeof(char));
                bufcp[buflen] = 0;
                strcpy(bufcp, buf);
                find(bufcp, regex);
                free(bufcp);
              }
            }
          }
          break;
        case T_DEVICE:
        case T_FILE:
          /* fprintf(2, "find: expected dir\n"); */
          break;

      }
      close(fd);
    }
  }
  return retcode;
}

int main(int argc, char *argv[]) {
  char *dir = argv[1];
  char *regex = argv[2];
  /* printf("regex: %s\n", regex); */

  int retcode = find(dir, regex);
  exit(retcode);
}
