#ifndef _BUFFER_H_
#define _BUFFER_H_

#define MAX_LENGTH 512

#define LINE_LENGTH 128

void init_buffer();

void put_buffer(char c);

char getLast();

char* getline();

#endif // _BUFFER_H_
