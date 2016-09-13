#include "buffer.h"
#include "kernelio.h"

char temp[MAX_LENGTH];
int p = 0, flag = 0;

void init_buffer() {
    temp[p] = '\0';
    p = 0;
}

void put_buffer(char c) {
    temp[p++] = c;
    temp[p] = '\0';
}

char getLast() {
    if (p != 0) {
        return temp[p-1];
    } else {
        return "";
    }
}

char* getline() {
    return temp;
}
