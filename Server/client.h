#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <pthread.h>

#define NUM_MAX_CONNECTIONS 10
#define AFK 0
#define ONLINE 1

typedef struct{
	int fd_client_socket;
	int cid;
	int status;
} client_t;

void add_client(client_t *clients[], client_t *client, pthread_mutex_t* muxtex_client);
void remove_client(client_t *clients[], int rm_cid, pthread_mutex_t* muxtex_client);

#endif //_CLIENT_H_