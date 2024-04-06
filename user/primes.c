// #include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"
int compute_prime(int fd);
int main(int argc, char const *argv[]) {
  /* code */

  int p[2];
  int status = pipe(p);
  if (status != 0) {
    fprintf(2, "11: error when creating the pipe: %d\n", status);
    exit(1);
  }
  int pid = fork();
  if (pid == 0) {
    close(p[1]);
    status = compute_prime(p[0]);
    if (status != 0) {
      fprintf(2, "18: error when compute prime: %d\n", status);
      exit(1);
    }
  } else {
    close(p[0]);
    for (char i = 2; i < 35; i++) {
      write(p[1], &i, sizeof(char));
    }
    close(p[1]);
    wait((int *)0);
  }
  return 0;
}

int compute_prime(int fd) {
  char p = 0;
  char n = 0;
  int pip[2] = {-1};
  for (;;) {
    int size;
    if (p == 0) {
      size = read(fd, &p, sizeof(char));
      printf("prime %d\n", p);
      continue;
    } else {
      size = read(fd, &n, sizeof(char));
    }
    if (size == -1) {
      fprintf(2, "46: error when reading from pipe: %d\n", size);
      exit(1);
    } else if (size == 0) {
      close(pip[1]);
      wait((int *)0);
      return 0;
    }
    if (n % p != 0) {
    ONPIPE:
      if (pip[0] != -1) {
        size = write(pip[1], &n, sizeof(char));
        if (size != 1) {
          fprintf(2, "57: error when reading from pipe: %d\n", size);
          exit(1);
        } else if (size == 0) {
          close(pip[1]);
        }
      } else {
        int status = pipe(pip);
        if (status != 0) {
          fprintf(2, "65: error when creating pipe: %d\n", status);
          exit(1);
        }
        int pid = fork();
        if (pid == 0) {
          close(pip[1]);
          compute_prime(pip[0]);
          exit(1);
        } else {
          close(pip[0]);
          goto ONPIPE;
        }
      }
    }
  }
}