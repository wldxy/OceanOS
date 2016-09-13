#include "memlib.h"
#include "type.h"
#include "memory.h"

static header_t* first = NULL;

static uint32_t mm_top = V_MM_START;

void* kmalloc(uint32_t size) {
    size += sizeof(header_t);

    header_t* node = first;
    header_t* prev = NULL;

    while (node != NULL) {
        if (node->used == 0 && node->length >= size) {
            if (node->length > sizeof(header_t) + size) {
                header_t* new_node = (header_t*)((uint32_t)node + node->length);
                new_node->prev = node;
                new_node->next = node->next;
                new_node->used = 0;
                new_node->length = node->length - size;
                node->next = new_node;
                node->length = size;
            }
            node->used = 1;
            return (void*)((uint32_t)node + sizeof(header_t));
        }
        prev = node;
        node = node->next;
    }

    uint32_t maddr;
    if (prev) {
        maddr = (uint32_t)prev + prev->length;
    } else {
        maddr = V_MM_START;
        first = (header_t*)maddr;
    }

    for (;mm_top < maddr + size; mm_top += PAGE_SIZE) {
        uint32_t page = alloc_page();
        map(pgd_kern, mm_top, page, PAGE_PRESENT | PAGE_WRITE);
    }

    node = (header_t*)maddr;
    node->prev = prev;
    node->next = NULL;
    node->used = 1;
    node->length = size;

    if (prev != NULL) {
        prev->next = node;
    }

    return (void*)(maddr + sizeof(header_t));
}

static void _free_memory(header_t* node);

void kfree(void* obj) {
    header_t* node = (header_t*)(uint32_t)obj - sizeof(header_t);
    node->used = 0;

    if (node->next && node->next->used == 0) {
        node->length = node->length + node->next->length;
        if (node->next->next != NULL) {
            node->next->next->prev = node;
        }
        node->next = node->next->next;
    }

    if (node->prev && node->prev->used == 0) {
        node->prev->length = node->length + node->prev->length;
        if (node->prev->prev != NULL) {
            node->prev->prev->next = node->prev;
        }
        node = node->prev;
    }

    if (node->next == NULL) {
        _free_memory(node);
    }
}

void _free_memory(header_t* node) {
    if (node->prev == NULL) {
        first = NULL;
    } else {
        node->prev->next = NULL;
    }

    for (;(mm_top - PAGE_SIZE) >= (uint32_t)node; mm_top -= PAGE_SIZE) {

        uint32_t page;
        get_mapping(pgd_kern, mm_top, &page);
        unmap(pgd_kern, mm_top);
        free_page(page);
    }
}

