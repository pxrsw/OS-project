#include "mydefs.h"
#include <ctype.h>
#include <pthread.h>

#define MAX_THREADS 5 //maximum number of threads that can run simultaneoulsy

//shared memories file descriptors
int shm_a_fd;
int shm_b_fd;

//pointers to beginning of shared memories
void *ptr_a;
void *ptr_b;

//pointers to shared memories' info structures
shm_info info_a;
shm_info info_b;

//named semaphores
sem_t *sem_signal_new_client;
sem_t *client_lock_a;
sem_t **sem_sig_req_ready_arr;
sem_t **sem_signal_response_arr;

//local semaphores
sem_t sem_available_threads;


void initialize_shared_memory();
void clean_up();

void *runner(void *param);
void write_answer();

void interuptHandler(int i) {
    printf("\ninterrupted!\ncleaning resources...\n");
    clean_up();
    printf("done!\n");
    exit(1);
}

int main(){

    signal(SIGINT, interuptHandler);

    clean_up();
    
    int clinet_counter = 0;

    initialize_shared_memory();
    printf("server is up and running!\n");

    sem_init(&sem_available_threads, 0, MAX_THREADS);

    shm_info *dummy_inf_a = &info_a;

    while(1) {
        sem_wait(sem_signal_new_client);
        
        int index = *(info_a.last_index);
        
        sem_post(client_lock_a);
        
        if (index >= MSG_BUFFER_SIZE){        //assert indext validity
            continue;
        }
        
        sem_wait(&sem_available_threads);
        pthread_t thread;
        pthread_create(&thread, NULL, runner, &index);
        printf("%d client(s) connected.\n", ++clinet_counter);
        fflush(stdout);
    }
}

void *runner(void *params) {
    int index = *(int*)params;
    shm_info *info = &info_a;
    sem_wait(sem_sig_req_ready_arr[index]);

    char *msg = read_msg_by_length(
        ptr_a + info_a.message_info_arr[index].msg_offset,
        info_a.message_info_arr[index].length);

    char *msg_backup = msg;
    while(*msg){
        *msg = toupper(*msg);
        msg++;
    }
    msg = msg_backup;
    void *ptr_dest = ptr_b + info_a.message_info_arr[index].msg_offset;
    memcpy(ptr_dest, msg, info_a.message_info_arr[index].length);

    sem_post(sem_signal_response_arr[index]);
    
    sem_post(&sem_available_threads);
    pthread_exit(0);
}

void initialize_shared_memory() {
    shm_a_fd = shm_open(SHM_NAME_A, O_CREAT | O_RDWR, 0666);
    shm_b_fd = shm_open(SHM_NAME_B, O_CREAT | O_RDWR, 0666);

    ftruncate(shm_a_fd, SHM_SIZE);
    ftruncate(shm_b_fd, SHM_SIZE);   

    ptr_a = mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_a_fd, 0);
    ptr_b = mmap(NULL, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_b_fd, 0);
    
    write_init_info(ptr_a);
    write_init_info(ptr_b);

    get_shm_info(&info_a, ptr_a);
    get_shm_info(&info_b, ptr_b);

    sem_signal_new_client = sem_open(SEM_NAME_SIGNAL_NEW_CLIENT, O_CREAT, 0644, 0);
    client_lock_a = sem_open(SEM_NAME_CLIENT_LOCK_A, O_CREAT, 0644, 1);
    
    sem_sig_req_ready_arr = (sem_t**)malloc(MSG_BUFFER_SIZE * sizeof(sem_t*));
    for (int i = 0; i < MSG_BUFFER_SIZE; i++){
        char name_index[6];
        sprintf(name_index, "%d", i);
        char name[strlen(SEM_BASE_NAME_SIGNAL_REQUEST_READY) + strlen(name_index) + 1];
        strcpy(name, SEM_BASE_NAME_SIGNAL_REQUEST_READY);
        strcat(name, name_index);
        sem_sig_req_ready_arr[i] = sem_open(name, O_CREAT, 0644, 0);
    }
    
    sem_signal_response_arr = (sem_t**)malloc(MSG_BUFFER_SIZE*sizeof(sem_t*));
    for (int i = 0; i < MSG_BUFFER_SIZE; i++)
    {
        char name_index[6];
        sprintf(name_index, "%d", i);
        char name[strlen(SEM_BASE_NAME_SIGNAL_RESPONSE) + strlen(name_index) + 1];
        strcpy(name, SEM_NAME_SIGNAL_NEW_CLIENT);
        strcat(name, name_index);
        sem_signal_response_arr[i] = sem_open(name, O_CREAT, 0644, 0);
    }
    
}


void clean_up(){
    sem_close(sem_signal_new_client);
    sem_destroy(&sem_available_threads);

    close(shm_a_fd);
    close(shm_b_fd);

    shm_unlink(SHM_NAME_A);
    shm_unlink(SHM_NAME_B);

    sem_unlink(SEM_NAME_CLIENT_LOCK_A);
    sem_unlink(SEM_NAME_SIGNAL_NEW_CLIENT);
    for (int i = 0; i < MSG_BUFFER_SIZE; i++){
        char name_index[6];
        sprintf(name_index, "%d", i);
        char name[strlen(SEM_BASE_NAME_SIGNAL_REQUEST_READY) + strlen(name_index) + 1];
        strcpy(name, SEM_BASE_NAME_SIGNAL_REQUEST_READY);
        strcat(name, name_index);
        sem_unlink(name);
    }
    
    for (int i = 0; i < MSG_BUFFER_SIZE; i++)
    {
        char name_index[6];
        sprintf(name_index, "%d", i);
        char name[strlen(SEM_BASE_NAME_SIGNAL_RESPONSE) + strlen(name_index) + 1];
        strcpy(name, SEM_NAME_SIGNAL_NEW_CLIENT);
        strcat(name, name_index);
        sem_unlink(name);
    }
}
