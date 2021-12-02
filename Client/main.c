#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

// My includes
#include "errono.h"
#include "my_string.h"

#define AFK 0
#define ONLINE 1

#define send_terminal_char()                                                   \
  pthread_mutex_lock(&mutex_rcv_buffer);                                       \
  printf(">");                                                                 \
  pthread_mutex_unlock(&mutex_rcv_buffer);                                     \
  fflush(stdout);

#define set_status(new_status)                                                 \
  pthread_mutex_lock(&mutex_status);                                           \
  status = new_status;                                                         \
  pthread_mutex_unlock(&mutex_status);

#define set_leave()                                                            \
  pthread_mutex_lock(&mutex_close);                                            \
  leave = 1;                                                                   \
  pthread_cond_signal(&cond_close);                                            \
  pthread_mutex_unlock(&mutex_close);

#define print(buffer)                                                          \
  pthread_mutex_lock(&mutex_rcv_buffer);                                       \
  pthread_create(&printf_thread, NULL, print_thread, (void *)buffer);          \
  pthread_detach(printf_thread);

// Server defines
#define SERVICE_PORT 5000
#define SERVICE_IP "127.0.0.1"

// other defines
#define SIGNAL_RCV 1 // signal received
#define SIGINT 2

// Global variables
volatile sig_atomic_t flag = 0;
int fd_socket = 0;
int leave = 0;
int status = 1;

pthread_t transmiter_thread;
pthread_t receiver_thread;
pthread_t close_thread;
pthread_t printf_thread;

struct itimerval itv;

/*
        Mutex and conditional variable for leave global function
*/
pthread_mutex_t mutex_close = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_close = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_status = PTHREAD_MUTEX_INITIALIZER;

/*
        Mutex for printf handler
*/
pthread_mutex_t mutex_printf = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t mutex_rcv_buffer = PTHREAD_MUTEX_INITIALIZER;

void *print_thread(void *arg) {
  char *msg = (char *)arg;
  pthread_mutex_lock(&mutex_printf);
  fflush(stdout);
  printf("%s\r\n", msg);
  pthread_mutex_unlock(&mutex_rcv_buffer);
  pthread_mutex_unlock(&mutex_printf);
  return 0;
}
/*
        This function is a hanler for the INTSIG signal (Control + C)
*/
void sig_handler(int sig) {
  switch (sig) {
  case SIGALRM:
    set_status(AFK) break;
  case SIGINT:
    print("\r\n Goodbye my lover... \r\n");
    set_leave();
    break;
  }
}

void *transmiter_handler(void *arg) {
  char message[BUFFER_SZ]; //= "default message\n";

  send(fd_socket, (char *)arg, string_lenght((char *)arg), 0);
  send_terminal_char();

  while (leave == 0) {
    fgets(message, BUFFER_SZ, stdin);
    add_termitor_to_string(message);
    send_terminal_char();
    setitimer(ITIMER_REAL, &itv, NULL);
    set_status(ONLINE);
    if (string_cmp(message, "exit") == VALID) {
      set_leave()
    } else {
      add_newline_to_string(message);
      send(fd_socket, message, string_lenght(message), 0);
    }
    string_clean(message);
  }
  sig_handler(SIGINT);
  return NULL;
}

void *receiver_handler(void *arg) {
  char message[BUFFER_SZ] = "";

  while (leave == 0) {
    int receive = recv(fd_socket, message, BUFFER_SZ, 0);
    if (receive > 0 && (string_cmp(message, "AWK") != VALID)) { //&& "AWK"

      print(message);
      send_terminal_char()
    } else if (receive > 0 && (string_cmp(message, "AWK") == VALID)) {
      if (status == ONLINE) {
        copy_string(5, message, "ACK 1");
      } else {
        copy_string(5, message, "ACK 0");
      }
      send(fd_socket, message, string_lenght(message), 0);
    } else if (receive == 0) {
      set_leave()
    } else {
      print("An error occur on receiver_handler()!");
      set_leave()
    }
    pthread_mutex_lock(&mutex_rcv_buffer);
    string_clean(message);
    pthread_mutex_unlock(&mutex_rcv_buffer);
  }
  return NULL;
}

void *close_handler(void *arg) {

  pthread_mutex_lock(&mutex_close);
  pthread_cond_wait(&cond_close, &mutex_close);

  // Befor close the socket wait for the transmiter and receiver
  // send and receive everything.
  // pthread_join(transmiter_thread, NULL);
  // pthread_join(receiver_thread, NULL);
  close(fd_socket);

  pthread_mutex_unlock(&mutex_close);
  return NULL;
}

int main(int argc, char *argv[]) {

  signal(SIGINT, sig_handler);
  signal(SIGALRM, sig_handler);

  if (argc != 2) {
    printf("Invalid number of arguments");
    return -1;
  }
  char *input_message = argv[1];

  struct sockaddr_in server_addr;
  /* Socket settings */
  fd_socket = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(SERVICE_IP);
  server_addr.sin_port = htons(SERVICE_PORT);
  // Connect to Server
  if (connect(fd_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    printf("ERROR on connecting to the server(%s) on port %d\n", SERVICE_IP, SERVICE_PORT);
    return ECONNREFUSED;
  } else {
    print("Connection established");
  }

  /*----------------------  Create Threads -------------------------*/

  if (pthread_create(&transmiter_thread, NULL, transmiter_handler,
                     (void *)input_message) != 0) {
    printf("ERROR on transmiter pthread\n");
    return EXIT_FAILURE;
  }

  if (pthread_create(&receiver_thread, NULL, receiver_handler, NULL) != 0) {
    printf("ERROR on receiver pthread\n");
    return EXIT_FAILURE;
  }

  if (pthread_create(&close_thread, NULL, close_handler, NULL) != 0) {
    printf("ERROR on close pthread\n");
    return EXIT_FAILURE;
  }
  /*------------------------ Start Timer ------------------------*/
  // Count 10 sec. After that the client is considered AFK.
  itv.it_interval.tv_sec = 10;
  itv.it_interval.tv_usec = 0;
  itv.it_value.tv_sec = 10;
  itv.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &itv, NULL);
  /*
          The pthread_join() function suspends execution of the
          calling thread until the target thread terminates, unless the
          target thread has already terminated
  */
  pthread_join(close_thread, NULL);

  /*
          By finishing the execution of main() by invoking pthread_exit()
          The process and all threads created by it will still run.
          But i prefer to use the pthread_join();
  */
  // pthread_exit(NULL);
}
