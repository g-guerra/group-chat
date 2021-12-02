#ifndef _MY_STRING_H_
#define _MY_STRING_H_

#define BUFFER_SZ 256

void copy_string(int lenght, char* rcv_buffer, char* src_buffer);
void add_termitor_to_string (char* buffer);
void add_newline_to_string (char* buffer);
int string_lenght(char* buffer);
void string_clean(char* buffer);
int string_cmp(char* buffer1, char* buffer2);

#endif //_MY_STRING_H_