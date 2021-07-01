#include "mydefs.h"
#include <sys/time.h>

sem_t *sem_signal_new_client;
sem_t *client_lock_a;
sem_t **sem_sig_req_ready_arr;
sem_t **sem_signal_response_arr;

void *ptr_a;
void *ptr_b;

int shm_a_fd;
int shm_b_fd;

void get_shm();

int main(int argc, char **argv) {
    
    char *msg;
    if (argc != 2) {
        printf("one text argument must be provided.\n");
        return 1;
    }
    msg = argv[1];

    get_shm();
    shm_info info_a;
    shm_info info_b;
    get_shm_info(&info_a, ptr_a);
    get_shm_info(&info_b, ptr_b);

    struct timeval stop, start;
    gettimeofday(&start, NULL);
    
    sem_wait(client_lock_a);
    
    size_t msg_length = strlen(msg);

    if (SHM_SIZE - *(info_a.next_msg_offset) < msg_length){
        printf("not enough space space on memory.");
        sem_post(client_lock_a);
        return 1;
    }

    int index = -1;
    for (int j = 0; j < MSG_BUFFER_SIZE; j++)
    {
        if(info_a.message_info_arr[j].free)    {
            index = j;
            break;
        }
    }
    if (index == -1) {
        printf("Message buffer is full.");
        sem_post(client_lock_a);
        return 1;
    }
    
    info_a.message_info_arr[index].free = false;
    info_a.message_info_arr[index].length = msg_length;
    info_a.message_info_arr[index].msg_offset = *(info_a.next_msg_offset);
    *(info_a.next_msg_offset) += msg_length * sizeof(char) + 1;
    *(info_a.last_index) = index;

    sem_post(sem_signal_new_client);

    memcpy(ptr_a + info_a.message_info_arr[index].msg_offset, msg, msg_length * sizeof(char) + 1);
    
    sem_post(sem_sig_req_ready_arr[index]);
    sem_wait(sem_signal_response_arr[index]);
    // char *rec_msg = read_msg_by_length(ptr_b + info_a.message_info_arr[index].msg_offset, msg_length);
    gettimeofday(&stop, NULL);
    printf("%s\n", ptr_b + info_a.message_info_arr[index].msg_offset);
    info_a.message_info_arr[index].free = true;
    printf("%lu micro seconds\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec); 
}

void get_shm() {
    shm_a_fd = shm_open(SHM_NAME_A, O_RDWR, 0666);
    shm_b_fd = shm_open(SHM_NAME_B, O_RDWR, 0666);

    ptr_a = mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_a_fd, 0);
    ptr_b = mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_b_fd, 0);
    if (ptr_a < 0){
        exit(1);
    }

    sem_signal_new_client = sem_open(SEM_NAME_SIGNAL_NEW_CLIENT, O_CREAT);
    client_lock_a = sem_open(SEM_NAME_CLIENT_LOCK_A, O_CREAT);

    sem_sig_req_ready_arr = (sem_t**)malloc(MSG_BUFFER_SIZE * sizeof(sem_t*));
    for (int i = 0; i < MSG_BUFFER_SIZE; i++){
        char name_index[6];
        sprintf(name_index, "%d", i);
        char name[strlen(SEM_BASE_NAME_SIGNAL_REQUEST_READY) + strlen(name_index) + 1];
        strcpy(name, SEM_BASE_NAME_SIGNAL_REQUEST_READY);
        strcat(name, name_index);
        sem_sig_req_ready_arr[i] = sem_open(name, O_CREAT);
    }

    sem_signal_response_arr = (sem_t**)malloc(MSG_BUFFER_SIZE*sizeof(sem_t*));
    for (int i = 0; i < MSG_BUFFER_SIZE; i++)
    {
        char name_index[6];
        sprintf(name_index, "%d", i);
        char name[strlen(SEM_BASE_NAME_SIGNAL_RESPONSE) + strlen(name_index) + 1];
        strcpy(name, SEM_NAME_SIGNAL_NEW_CLIENT);
        strcat(name, name_index);
        sem_signal_response_arr[i] = sem_open(name, O_CREAT);
    }
    
}