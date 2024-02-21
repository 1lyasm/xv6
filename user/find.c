#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

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


int find(char *dir, char *fname) {
  /* printf("find: dir: %s, fname: %s\n", dir, fname); */
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
              /* printf("buf: %s, p: %s, fname: %s\n", buf, p, fname); */
              if (strcmp(p, fname) == 0) {
                fprintf(1, "%s\n", buf);
              }
              if (strcmp(p, ".") != 0 && strcmp(p, "..") != 0) {
                int buflen = strlen(buf);
                char *bufcp = malloc((buflen + 1) * sizeof(char));
                bufcp[buflen] = 0;
                strcpy(bufcp, buf);
                find(bufcp, fname);
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
  char *fname = argv[2];
  /* printf("fname: %s\n", fname); */

  int retcode = find(dir, fname);
  exit(retcode);
}
