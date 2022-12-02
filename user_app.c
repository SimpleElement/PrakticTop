#include "character_dev.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char **argv)
{
    int fd;

    if (argc < 2 || (strcmp(argv[1], "help") == 0))
    {
        printf("Usage %s <PID> <PAGE-NUMBER>\n", argv[0]);
        printf("<PID> must be integer more than zero\n");

        return 0;
    }
    struct lab_data *ld = malloc(sizeof(struct lab_data));
    ld->pid = atoi(argv[1]);
    if (ld->pid)
    {
        printf("Entered <PID> is %d\n", ld->pid);
    }
    else
    {
        printf("Wrong <PID>\n");
        return 1;
    }


    fd = open(DEVICE_NAME, 0);

    if (fd < 0)
    {
        printf("Can't open device file: %s\n", DEVICE_NAME);
        exit(2);
    }

    int ret_val = ioctl(fd, IOCTL_GET_INFO, ld);
    if (ret_val != 0)
    {
        printf("IOCTL_GET_INFO failed %d: process with <PID> = %d doesn't exist\n", ret_val,ld->pid);
        exit(ret_val);
    }
    printf("<-- TASK STRUCT -->\n");
    printf("FOR TASK_STRUCT WITH PID = %d\n", ld->pid);
    printf("TDIG = %d\n", ld->result.tgid);
    printf("FLAGS = %x\n", ld->result.task_flags);

    printf("<-- SIGNAL STRUCT -->\n");
    printf("FOR SIGNAL_STRUCT WITH PID = %d\n", ld->pid);
    printf("FLASG = %x\n", ld->result.sig_flags);
    printf("sig_group_stop_count = %x\n", ld->result.sig_group_stop_count);
    printf("sig_group_exit_code = %x\n", ld->result.sig_group_exit_code);
    return 0;
}
