#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
    int parent_to_child[2];
    int child_to_parent[2];
    int status = pipe(parent_to_child);
    if (status != 0){
        fprintf(2,"making pipe fail: %d",status);
        exit(1);
    }
    status = pipe(child_to_parent);
    if (status != 0){
        fprintf(2,"making pipe fail: %d",status);
        exit(1);
    }
    char *b = "";
    int pid = fork();
    if (pid == 0){
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        int size = read(parent_to_child[1], (int *) 0,1);
        if (size != -1){
            fprintf(2,"reading from parent pipe fail: %d\n", status);
            exit(1);
        }
        int pid = getpid();
        printf("%d: received ping\n",pid);
        size = write(child_to_parent[0],b, 1);
        if (size != -1){
            fprintf(2,"sending to parent pipe fail: %d\n", status);
            exit(1);
        };
        exit(0);
    }else{
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        int size = write(parent_to_child[0],b,1);
        if (size != -1){
            fprintf(2,"sending to child pipe fail: %d\n", status);
            exit(1);
        };
        size = read(child_to_parent[1],(int *) 0,1);
        if (size != -1){
            fprintf(2,"reading to parent pipe fail: %d\n", status);
            exit(1);
        };
        wait((int *) 0);
        int pid = getpid();
        printf("%d: received pong\n",pid);
        exit(0);

    }
    return 0;
}
