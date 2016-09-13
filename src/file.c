#include "file.h"
#include "system.h"
#include "string.h"
#include "type.h"

file file_list[MAX_FILE];

static uint32_t file_id = 0;

/*
 *  向内核文件表中添加一个目录
 *  如果是个伪文件，ftime置0即可
 */
int32_t fs_add_dir(uint32_t father, uint32_t ftime, int8_t *name)
{
    uint32_t eflags;
    _local_irq_save(eflags);
    if (file_id < MAX_FILE)
    {
        file_list[file_id].type = F_T_DIR;
        file_list[file_id].father = father;
        file_list[file_id].time = ftime;
        file_list[file_id].base = 0;
        file_list[file_id].size = 0;
        file_list[file_id].f_op = 0;
        file_list[file_id].f_req = 0;

        strcpy(file_list[file_id].name, name);

        file_list[father].size++;
        file_id++;

        _local_irq_restore(eflags);
        return file_id-1;
    }
    else
    {
        _local_irq_restore(eflags);
        return -1;
    }

}

/*
 *  向内核文件表中添加一个文件
 *  如果是个伪文件，ftime置0即可
 */
int32_t fs_add_file(uint32_t father, uint32_t ftime, uint32_t base, uint32_t size,
    struct _file_operations *f_op,
    struct _file_req *f_req,
    char *name)
{
    uint32_t i,eflags;
    _local_irq_save(eflags);
    if (file_id < MAX_FILE)
    {

        file_list[file_id].type = F_T_FILE;
        file_list[file_id].father = father;
        file_list[file_id].time = ftime;
        file_list[file_id].base = base;
        file_list[file_id].size = size;
        file_list[file_id].f_op = f_op;
        file_list[file_id].f_req = f_req;
        for (i=0;i<MAX_TASK;i++) file_list[file_id].pos[i] = 0;
        strcpy(file_list[file_id].name, name);

        file_list[father].size++;
        file_id++;
        _local_irq_restore(eflags);
        return file_id-1;
    }
    else
    {
        _local_irq_restore(eflags);
        return -1;
    }
}


int32_t  fs_match_dir(uint32_t father, char *path)
{
    uint32_t i;
    for (i = 1; i < file_id; i++)
    {
        if( file_list[i].type   == F_T_DIR && \
            file_list[i].father == father && \
            0 == strncmp(path, file_list[i].name, strlen(file_list[i].name)))
        {
            return i;
        }
    }
    return -1;
}


int32_t  fs_match_file(uint32_t father, char *name)
{
    uint32_t i;
    for (i = 1; i < file_id; i++)
    {
        if( file_list[i].type   == F_T_FILE && \
            file_list[i].father == father && \
            0 == strcmp(name, file_list[i].name))
        {
            return i;
        }
    }
    return -1;
}

void init_fs()
{
    file_id = 0;
    fs_add_dir(0, 0, "*");
    fs_add_dir(0, 0, "/");
    fs_add_dir(1, 0, "dev/");
    fs_add_dir(1, 0, "sys/");
}

