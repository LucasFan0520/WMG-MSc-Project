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

Item *find_item(const char *name) {
    Item *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_item(const char *name, int count) {
    Item *i = find_item(name);
    if (i) {
        i->count += count;
    } else {
        i = malloc(sizeof(Item));
        if (!i) exit(EXIT_FAILURE);
        i->name = strdup(name);
        i->count = count;
        i->next = NULL;
        if (!head) {
            head = i;
        } else {
            Item *curr = head;
            while (curr->next) curr = curr->next;
            curr->next = i;
        }
    }
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    i->count -= count;
    if (i->count < 0) i->count = 0;
}

void remove_item(const char *name) {
    Item *curr = head;
    Item *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void count_item(const char *name) {
    Item *i = find_item(name);
    if (i) printf("%d\n", i->count);
    else printf("0\n");
}

void report_items() {
    Item *curr = head;
    while (curr) {
        printf("%s %d\n", curr->name, curr->count);
        curr = curr->next;
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char name[256] = {0};
        int count = 0;
        
        int matched = sscanf(line, "%31s %255s %d", cmd, name, &count);
        
        if (strcmp(cmd, "ADD") == 0 && matched == 3) {
            add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0 && matched == 3) {
            take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}