#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
    if (argc == 1){
        fprintf(2, "sleep: missing operand");
        exit(1);
    }
    for (int i = 1 ;i < argc; i++){
        int sleep_duration = atoi(argv[i]);
        sleep(sleep_duration);
    }
    exit(0);
    return 0;
}
