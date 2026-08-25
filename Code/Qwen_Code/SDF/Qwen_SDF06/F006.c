// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *head = NULL;

void add_item(const char *name, int count) {
    for (Item *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            curr->count += count;
            return;
        }
    }
    Item *i = malloc(sizeof(Item));
    if (!i) return;
    i->name = strdup(name);
    i->count = count;
    i->next = head;
    head = i;
}

void take_item(const char *name, int count) {
    for (Item *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            curr->count -= count;
            if (curr->count < 0) curr->count = 0;
            return;
        }
    }
}

void remove_item(const char *name) {
    Item *curr = head;
    Item *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            Item *next = curr->next;
            free(curr->name);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void count_item(const char *name) {
    for (Item *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            printf("%d\n", curr->count);
            return;
        }
    }
    printf("0\n");
}

void report_items() {
    for (Item *curr = head; curr; curr = curr->next) {
        printf("%s %d\n", curr->name, curr->count);
    }
}

void free_all() {
    Item *curr = head;
    while (curr) {
        Item *next = curr->next;
        free(curr->name);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char name[1024];
        int count;
        
        int n = sscanf(line, "%15s %1023s %d", cmd, name, &count);
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0 && n == 3) {
            take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0 && n >= 2) {
            count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}