// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Item {
    char* name;
    int count;
    struct Item* next;
} Item;

Item* head = NULL;

Item* find_item(const char* name) {
    for (Item* i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) return i;
    }
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "ADD") == 0) {
            char* name = get_word(&p);
            int c = atoi(p);
            Item* i = find_item(name);
            if (i) i->count += c;
            else {
                Item* n = malloc(sizeof(Item));
                n->name = strdup(name);
                n->count = c;
                n->next = NULL;
                if (!head) head = n;
                else {
                    Item* tail = head;
                    while (tail->next) tail = tail->next;
                    tail->next = n;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char* name = get_word(&p);
            int c = atoi(p);
            Item* i = find_item(name);
            if (i) {
                i->count -= c;
                if (i->count < 0) i->count = 0;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char* name = get_word(&p);
            Item* prev = NULL;
            for (Item* i = head; i; prev = i, i = i->next) {
                if (strcmp(i->name, name) == 0) {
                    if (prev) prev->next = i->next;
                    else head = i->next;
                    free(i->name);
                    free(i);
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char* name = get_word(&p);
            Item* i = find_item(name);
            printf("%d\n", i ? i->count : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Item* i = head; i; i = i->next) {
                printf("%s %d\n", i->name, i->count);
            }
        }
    }
    return 0;
}