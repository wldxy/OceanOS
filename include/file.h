#ifndef _FILE_H_
#define _FILE_H_

#include "type.h"

#define F_T_FILE                0
#define F_T_DIR                 1
#define F_T_DEV                 2

#define REQ_FREE                0
#define REQ_BUSY                1
#define REQ_COMP                2

#define DIR_TOP                 0
#define DIR_ROOT                1
#define DIR_DEV                 2
#define DIR_SYS                 3

#define SEEK_SET                0
#define SEEK_CUR                1
#define SEEK_END                2

#define BUFFER_SIZE             512
#define MAX_NAME                20
#define MAX_TASK                32
#define MAX_FILE                128

typedef struct _file_operations {
    int32_t (*seek) (void *, int32_t, uint32_t);             //定位文件当前的读写位置
    int32_t (*read) (void *, uint8_t *, uint32_t);            //读文件
    int32_t (*write) (void *, uint8_t *, uint32_t);           //写文件
}__attribute__((packed)) file_operations;

typedef struct _file {
    uint32_t type;
    uint32_t father;                               //父目录索引
    uint32_t time;                                 //最后修改时间
    uint32_t base;                                 //文件基址，不同的文件含义不同
    int32_t size;                                 //文件大小
    int32_t pos[MAX_TASK];                        //读写位置,每个任务有自己的读写位置
    struct _file_operations *f_op;              //操作文件跳转指针 不能空！！！
    struct _file_req *f_req;                    //操作请求结构，可以空
    char  name[MAX_NAME];
}__attribute__((packed)) file;

typedef struct _file_req
{
    uint8_t  f_req_state;                            //设备状态
    uint32_t f_req_orgsize;                          //原始请求大小
    uint32_t f_req_completed;                        //已完成请求大小
    uint32_t f_req_task;                             //发起请求的任务id
    uint8_t  f_req_buffer[BUFFER_SIZE];
}__attribute__((packed)) file_req;

void init_fs();
int32_t  fs_add_dir(uint32_t father, uint32_t ftime, char *name);
int32_t  fs_add_file(uint32_t father, uint32_t ftime,
                            uint32_t base, uint32_t size,
                            struct _file_operations *f_op,
                            struct _file_req *f_req_read,
                            char *name);

int32_t  fs_match_dir(uint32_t father, char *path);
int32_t  fs_match_file(uint32_t father, char *name);

extern file file_list[MAX_FILE];

#endif // _FILE_H_
