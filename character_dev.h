
#ifndef CHARACTER_DEV_H
#define CHARACTER_DEV_H

#include <linux/ioctl.h>


#define MAJOR_NUM 100


#define IOCTL_GET_INFO _IOR(MAJOR_NUM, 2, char *)


struct result
{
    int tgid;
    unsigned int task_flags;
    unsigned int sig_flags;
    int sig_group_exit_code;
    int sig_group_stop_count;
};
struct lab_data
{
    int pid;
    struct result result;
};

#define DEVICE_NAME "lab_character_device"
#define BUFFER_SIZE 1024
#endif