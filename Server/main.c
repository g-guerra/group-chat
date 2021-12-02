
// Sys includes
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// My modules includes
#include "client.h"
#include "errono.h"
#include "my_string.h"

// Server defines
#define SERVICE_PORT 5000
#define SERVICE_IP "127.0.0.1"
#define SEND_TO_ALL -1

typedef struct sockaddr_in sockaddr_in_t;

pthread_mutex_t muxtex_client = PTHREAD_MUTEX_INITIALIZER;
client_t *clients[NUM_MAX_CONNECTIONS];

/*
    This function is responsible for the server socket creation and
   initialization
*/
int init_default_socket(int *fd_socket, sockaddr_in_t socket_addr) {
  // Create the server socket
  // PF_INET -> Protocol IPv4
  // SOCK_STREAM -> Use TCP connection
  //
  // EBADFD - File descriptor in bad state
  *fd_socket = socket(PF_INET, SOCK_STREAM, 0);
  if (*fd_socket < 0) {
    printf("ERROR EBADFD! Creating socket\r\n");
    return EBADFD;
  }

  // Prepare the data to be ninded to the socket
  memset(&socket_addr, 0, sizeof(socket_addr)); // set socket_addr to 0
  socket_addr.sin_addr.s_addr = inet_addr(SERVICE_IP);
  socket_addr.sin_port = htons(SERVICE_PORT);
  socket_addr.sin_family = AF_INET;

  // Bind the Port and Address to the server socket
  if (bind(*fd_socket, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) !=
      0) {
    printf("ERROR! Binding info to socket\r\n");
    return EPROTO;
  }

  // Make the socket a listener
  if (listen(*fd_socket, NUM_MAX_CONNECTIONS) != 0) {
    printf("ERROR! Making socket a listener\r\n");
    return EPROTO;
  }

  return VALID;
}

/*
    This function will echo a received message for all active clients less the
    message source client.
*/
void message_echo(char *buffer, int src_cid) {
  pthread_mutex_lock(&muxtex_client);

  for (int i = 0; i < NUM_MAX_CONNECTIONS; i++) {
    if ((clients[i] != NULL) && (clients[i]->cid != src_cid)) {
      send(clients[i]->fd_client_socket, buffer, string_lenght(buffer), 0);
    }
  }

  pthread_mutex_unlock(&muxtex_client);
}

/*
    Worker to hanlde the communications of each client
*/
void *handle_client(void *arg) {
  char buff_out[BUFFER_SZ];
  char mssg[BUFFER_SZ * 2];
  int leave_flag = 0;
  int handler_online = -1;
  int hanlder_akf = -1;

  client_t *cli = (client_t *)arg;

  recv(cli->fd_client_socket, buff_out, BUFFER_SZ, 0);
  printf("CID Join: %d\r\n", cli->cid);
  message_echo(buff_out, cli->cid);

  string_clean(buff_out, BUFFER_SZ);

  while (leave_flag != 1) {

    int receive = recv(cli->fd_client_socket, buff_out, BUFFER_SZ, 0);
    handler_online = string_cmp(buff_out, "ACK 1");
    hanlder_akf = string_cmp(buff_out, "ACK 0");

    if ((receive > 0) && (handler_online != VALID) && (hanlder_akf != VALID)) {

      sprintf(mssg, "Client %d say: %s", cli->cid, buff_out);
      message_echo(mssg, cli->cid);
      add_termitor_to_string(mssg);
      printf(mssg);

    } else if ((receive > 0) && (handler_online == VALID)) {

      cli->status = ONLINE;
      printf("Client %d is online(%d)\r\n", cli->cid, cli->status);

    } else if ((receive > 0) && (hanlder_akf == VALID)) {

      cli->status = AFK;
      printf("Client %d is AFK(%d)\r\n", cli->cid, cli->status);

    } else if (receive == 0 || string_cmp(buff_out, "exit") == VALID) {

      sprintf(buff_out, "... Goodbye my friend ... \r\n CID left %d\r\n",
              cli->cid);
      printf("%s", buff_out);
      message_echo(buff_out, cli->cid);
      leave_flag = 1;

    } else {
      printf("ERROR on recv system call from client %d\n", cli->cid);
      leave_flag = 1;
    }

    string_clean(buff_out, BUFFER_SZ);
  }

  // close the client socket
  close(cli->fd_client_socket);
  // remove the client from the list of clients
  remove_client(clients, cli->cid, &muxtex_client);
  free(cli);
  pthread_detach(pthread_self());

  return 0;
}

static void periodic_check(int sig) {
  if (sig == SIGALRM) {
    // printf("heelo\r\n");
    message_echo("AWK", SEND_TO_ALL);
  }
}

int main(int arg, char *args[]) {

  struct sockaddr_in srv_addr;
  struct sockaddr_in cli_addr;
  int fd_srv;
  int g_cid = 0;

  struct itimerval itv;

  signal(SIGALRM, periodic_check);

  for (int i = 0; i < NUM_MAX_CONNECTIONS; i++) {
    clients[i] = NULL;
  }

  itv.it_interval.tv_sec = 5;
  itv.it_interval.tv_usec = 0;
  itv.it_value.tv_sec = 5;
  itv.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &itv, NULL);

  if (init_default_socket(&fd_srv, srv_addr) != VALID)
    return -1;

  // Look for connections
  while (1) {
    pthread_t tid;
    socklen_t addr_lenght = sizeof(cli_addr);
    int fd_new_socket =
        accept(fd_srv, (struct sockaddr *)&cli_addr, &addr_lenght);

    client_t *new_client = (client_t *)malloc(sizeof(client_t));
    new_client->cid = g_cid++;
    new_client->fd_client_socket = fd_new_socket;

    add_client(clients, new_client, &muxtex_client);
    pthread_create(&tid, NULL, handle_client, (void *)new_client);
  }

  return 0;
}
