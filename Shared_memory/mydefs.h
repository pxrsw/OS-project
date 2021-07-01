#ifndef __MYDEFSHM_H__
#define __MYDEFSHM_H__

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/shm.h> 
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>

#define SHM_SIZE 1048576 //shared memory size

//shared mem name  
#define SHM_NAME_A "/shm_a"
#define SHM_NAME_B "/shm_b"

//semaphore name
#define SEM_NAME_CLIENT_LOCK_A "/sem_client_lock_a"
#define SEM_NAME_SIGNAL_NEW_CLIENT "/sem_sig_new_client"
#define SEM_BASE_NAME_SIGNAL_REQUEST_READY "/sem_sig_req_ready_"
#define SEM_BASE_NAME_SIGNAL_RESPONSE "/sem_sig_response_"

#include "shm_info.h"

#endif