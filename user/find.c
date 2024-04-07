#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
void find(const char *path, const char *target);
char *strstr(const char *str, const char *substring);
int main(int argc, char const *argv[]) {
  if (argc < 2 || argc > 4) {
    fprintf(2, "find: invalid number of argument");
    exit(1);
  }
  find(argv[1], argv[2]);
  return 0;
}

void find(const char *path, const char *target) {
  int fd;
  struct dirent de;
  struct stat st;
  char buffer[512];
  char *p;
  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  if (st.type != T_DIR) {
    // fprintf(2, "find: cannot find file: %s\n", path);
    // close(fd);
    return;
  }
  strcpy(buffer, path);
  p = buffer + strlen(path);
  *p++ = '/';
  while (read(fd, &de, sizeof de) == sizeof de) {
    if (strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0) continue;
    if (de.inum == 0) {
      continue;
    }
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if (stat(buffer, &st) < 0) {
      fprintf(2, "find: cannot stat file: %s", buffer);
      exit(1);
      return;
    }
    if (st.type == T_FILE) {
      if (strstr(de.name, target) != 0) {
        printf("%s\n", buffer);
      }
    } else if (st.type == T_DIR) {
      const char *b = buffer;
      find(b, target);
    }
    // ls(buffer);
    // printf("%s\n", de.name);
  }
}

char *strstr(const char *str, const char *substring) {
  const char *a = str, *b = substring;
  for (;;) {
    if (!*b) return (char *)str;
    if (!*a) return 0;
    if (*a++ != *b++) {
      a = ++str;
      b = substring;
    }
  }
}
