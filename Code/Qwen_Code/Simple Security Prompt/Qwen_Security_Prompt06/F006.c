// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *head = NULL;
Item *tail = NULL;

Item *find_item(const char *name) {
    for (Item *i = head; i; i = i->next) {
        if (strcmp(i->name, name) == 0) return i;
    }
    return NULL;
}

void add_item(const char *name, int count) {
    Item *i = find_item(name);
    if (i) {
        i->count += count;
    } else {
        i = malloc(sizeof(Item));
        i->name = strdup(name);
        i->count = count;
        i->next = NULL;
        if (tail) tail->next = i;
        else head = i;
        tail = i;
    }
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (i) {
        i->count -= count;
        if (i->count < 0) i->count = 0;
    }
}

void remove_item(const char *name) {
    Item *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, name[256] = {0};
        int count = 0;
        sscanf(line, "%15s %255s %d", cmd, name, &count);
        
        if (strcmp(cmd, "ADD") == 0) {
            add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0) {
            take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            Item *i = find_item(name);
            printf("%d\n", i ? i->count : 0);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Item *i = head; i; i = i->next) {
                printf("%s %d\n", i->name, i->count);
            }
        }
        free(line);
    }
    
    Item *i = head;
    while (i) {
        Item *next = i->next;
        free(i->name);
        free(i);
        i = next;
    }
    return 0;
}