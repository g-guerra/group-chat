#include "my_string.h"
#include "errono.h"

void copy_string(int lenght, char* rcv_buffer, char* src_buffer){
    int i;

    for(i = 0; i < lenght; i++){
        rcv_buffer[i] = src_buffer[i];
    }

    rcv_buffer[i] = '\0';
}

void add_termitor_to_string (char* buffer) {
  int i;
  int lenght = string_lenght(buffer);

  for (i = 0; i < lenght; i++) { 
    if (buffer[i] == '\n') {
      buffer[i] = '\0';
    }
  }
}

int string_lenght(char* buffer){
  int i;
  
  for (i = 0; i < BUFFER_SZ; i++) { 
    if (buffer[i] == '\0') {
      return i;
    }
  }
  return ENODATA;
}

void string_clean(char* buffer, int buffer_lenght){
  int i;
  
  for (i = 0; i < buffer_lenght; i++) { 
    buffer[i] = '\0';
  }

}

int string_cmp(char* buffer1, char* buffer2){
  int lenght = string_lenght(buffer1);

  for(int i = 0; i < lenght; i++){
    if(*buffer1 != *buffer2)
      return ENOMSG;

    buffer1++;
    buffer2++;
  }
  return VALID;
}