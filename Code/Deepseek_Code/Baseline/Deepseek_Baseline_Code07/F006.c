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
Item *tail = NULL;

Item *find_item(const char *name) {
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) return i;
        i = i->next;
    }
    return NULL;
}

void add_item(const char *name, int count) {
    Item *i = find_item(name);
    if (i) {
        i->count += count;
    } else {
        Item *new = malloc(sizeof(Item));
        new->name = malloc(strlen(name)+1);
        strcpy(new->name, name);
        new->count = count;
        new->next = NULL;
        if (tail) tail->next = new;
        else head = new;
        tail = new;
    }
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    i->count -= count;
    if (i->count < 0) i->count = 0;
}

void remove_item(const char *name) {
    Item *prev = NULL;
    Item *i = head;
    while (i) {
        if (strcmp(i->name, name) == 0) {
            if (prev) prev->next = i->next;
            else head = i->next;
            if (i == tail) tail = prev;
            free(i->name);
            free(i);
            return;
        }
        prev = i;
        i = i->next;
    }
}

void count_item(const char *name) {
    Item *i = find_item(name);
    printf("%d\n", i ? i->count : 0);
}

void report_items(void) {
    Item *i = head;
    while (i) {
        printf("%s %d\n", i->name, i->count);
        i = i->next;
    }
}

void free_all(void) {
    Item *i = head;
    while (i) {
        Item *next = i->next;
        free(i->name);
        free(i);
        i = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *cnt = strtok(NULL, " ");
            int c = cnt ? atoi(cnt) : 0;
            add_item(name, c);
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *cnt = strtok(NULL, " ");
            int c = cnt ? atoi(cnt) : 0;
            take_item(name, c);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *name = strtok(rest, " ");
            if (name) remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *name = strtok(rest, " ");
            if (name) count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    return 0;
}