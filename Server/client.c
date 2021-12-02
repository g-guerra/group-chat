#include "client.h"


/* Add clients to queue */
void add_client(client_t *clients[], client_t *client, pthread_mutex_t* muxtex_client){
    pthread_mutex_lock(muxtex_client);
    int i = 0;
    int leave = 0;

    while((i < NUM_MAX_CONNECTIONS) && (leave == 0)){
        if(clients[i] == NULL){
			clients[i] = client;
            leave = 1;
		}
        i++;
    }
    pthread_mutex_unlock(muxtex_client);
}

/* Remove clients to queue */
void remove_client(client_t *clients[], int rm_cid, pthread_mutex_t* muxtex_client){
    pthread_mutex_lock(muxtex_client);
	for(int i=0; i < NUM_MAX_CONNECTIONS; i++){
		if((clients[i] != NULL) && (clients[i]->cid == rm_cid)){
			clients[i] = NULL;
		}
	}
    pthread_mutex_unlock(muxtex_client);
}