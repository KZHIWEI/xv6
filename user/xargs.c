#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define MAXARGLENGTH 32
int main(int argc, char const *argv[]) {
  char buf[MAXARG][MAXARGLENGTH] = {0};
  memset(buf, 0, sizeof buf);
  char input_buffer[MAXARG * MAXARGLENGTH] = {0};
  if (argc > 2) {
    for (int i = 2; i < argc; i++) {
      strcpy(buf[i - 2], argv[i]);
      buf[i - 2][strlen(argv[i])] = '\0';
    }
  }
  int size = 0;
  char *head = input_buffer;
  while ((size = read(0, head, MAXARGLENGTH)) > 0) {
    head += size;
  }
  //   int size = read(0, input_buffer, MAXARGLENGTH * MAXARG);
  //   if (size == -1) {
  //     exit(1);
  //   }
  //   printf("%s\n", input_buffer);
  char *p = input_buffer;
  char *begin = input_buffer;
  int i = argc - 2;
  do {
    if (*p == '\n' || *p == ' ') {
      memcpy(buf[i], begin, p - begin);
      buf[i][p - begin + 1] = '\0';
      begin = p + 1;
      i++;
    }
  } while (*p++);
  //   for (int i = 0; i < 32; i++) {
  //     printf("%d: %s\n", i, buf[i]);
  //   }
  char *result[MAXARG];
  char s[strlen(argv[1]) + 1];
  memcpy(s, argv[1], strlen(argv[1]));
  s[strlen(argv[1])] = '\0';
  result[0] = s;
  for (int c = 1; c < i + 1; c++) {
    result[c] = buf[c - 1];
  }
  result[i + 1] = 0;
  //   for (int i = 0; result[i] && i < 32; i++) {
  //     printf("%d: %s\n", i, result[i]);
  //   }
  if (fork() == 0) {
    int status = exec(argv[1], result);
    if (status != 0) {
      fprintf(2, "xargs: invalid exec '%s': %d", argv[1], status);
      exit(1);
    }
  } else {
    wait((int *)0);
  }
  return 0;
}
