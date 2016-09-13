#include "memory.h"
#include "type.h"
#include "monitor.h"
#include "debug.h"
#include "gdt.h"
#include "trap.h"
#include "memory.h"
#include "multiboot.h"
#include "keyboard.h"
#include "memlib.h"
#include "process.h"
#include "string.h"
#include "system.h"

void showStart() {
    int i, j;

    monitor_clear();
    for (i = 0;i < 30;i++) {
        printf("===============Welcome to OceanOS===============\n");

        printf("    Init system\n");
        for (j = 0;j <= i;j++) {
            printf("==");
        }
        sleep(20);

        monitor_clear();
    }
    showMemu();
}

void showMemu() {
    monitor_clear();

    printf("f1 ----------- test memory\n");
    printf("f2 ----------- test process\n");
}

void testMalloc() {
    int flag = 1, p = 0;
    char code[20];
    int size, id;
    void* item[20];

    sti();
    monitor_clear();
    printf("Memory malloc and free test\n");
    while (flag) {
        printf(" your code: ");
        scanf("%s", code);
        if (!strcmp(code, "malloc")) {
            printf("please input size : ");
            scanf("%d", &size);
            item[p] = kmalloc(size);
            printf("the %d memory start from 0x%X\n\n", size, item[p]);
            p++;
        }
        if (!strcmp(code, "free")) {
            printf("please input free item : ");
            scanf("%d", &id);
            if (item[id] != NULL) {
                kfree(item[id]);
                printf("the memory free at 0x%X\n\n", item[id]);
            }

        }
        if (!strcmp(code, "exit")) {
            flag = 0;
        }
    }
    showMemu();
}

int testA(void* str) {
    int k = 0;
    while (k++ < 15) {
        printf(rc_red, rc_red, " * proc A : %s  %d* \n", (char*)str, k);
        sleep(2);
    }

}

int testB(void* str) {
    int k = 0;
    while (k++ < 15) {
        printf(rc_green, rc_green, " * proc B : %s  %d* \n", (char*)str, k);
        sleep(2);
    }
}

int testC(void* str) {
    int k = 0;
    while (k++ < 15) {
        printf_color(rc_blue, rc_blue, " * proc C : %s  %d* ", (char*)str, k);
        sleep(2);
    }
}


void testProcess() {
    char strA[20], strB[20], strC[20];

    cli();
    init_timer(5);
    sti();

    monitor_clear();
    printf("process test\n");

    printf("Process A print string : \n");
    scanf("%s", strA);
    printf("\n");

    printf("Process B print string : \n");
    scanf("%s", strB);
    printf("\n");

    printf("Process C print string : \n");
    scanf("%s", strC);
    printf("\n");

    init_thread(testA, strA);
    init_thread(testB, strB);
    init_thread(testC, strC);

    //showMemu();

}


