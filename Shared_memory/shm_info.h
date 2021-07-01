#ifndef __SHM_INFO_H__
#define __SHM_INFO_H__

#include <stdbool.h>
#define MSG_BUFFER_SIZE 10

typedef struct struct_msg_info
{   
    bool free;
    size_t msg_offset;
    size_t length;
}msg_info;

typedef struct struct_shm_info
{
    msg_info *message_info_arr;
    size_t *next_msg_offset;
    int *last_index;
}shm_info;

void write_init_info(void *shm_ptr) {
    size_t next_msg_offset = sizeof(msg_info) * MSG_BUFFER_SIZE + sizeof(int) + sizeof(size_t);
    int last_index = 0;
    for(int i=0; i <MSG_BUFFER_SIZE; i++){
        msg_info m_info;
        m_info.free = true;
        memcpy(shm_ptr, &m_info, sizeof(msg_info));
        shm_ptr += sizeof(msg_info);
    }
    memcpy(shm_ptr, &next_msg_offset, sizeof(size_t));
    shm_ptr += sizeof(size_t);
    memcpy(shm_ptr, &last_index, sizeof(int));
}

void get_shm_info(shm_info *info_ptr, void *shm_ptr) {
    info_ptr->message_info_arr = shm_ptr;
    shm_ptr += MSG_BUFFER_SIZE * sizeof(msg_info);
    info_ptr->next_msg_offset = shm_ptr;
    shm_ptr += sizeof(*(info_ptr->next_msg_offset));
    info_ptr->last_index = shm_ptr;
}


char* read_msg_by_length(void *src_ptr, size_t len) {
    char *msg;
    msg = (char*)malloc(len*sizeof(char) + 1);
    sprintf(msg, "%s", (char*)src_ptr);
    return msg;
}

#endif