// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char *name;
    int count;
    struct Item *next;
} Item;

Item *head = NULL, *tail = NULL;

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
        new->name = malloc(strlen(name) + 1);
        strcpy(new->name, name);
        new->count = count;
        new->next = NULL;
        if (tail) {
            tail->next = new;
            tail = new;
        } else {
            head = tail = new;
        }
    }
}

void take_item(const char *name, int count) {
    Item *i = find_item(name);
    if (!i) return;
    if (i->count < count) i->count = 0;
    else i->count -= count;
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *cnt = strtok(NULL, " ");
            if (name && cnt) add_item(name, atoi(cnt));
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *name = strtok(NULL, " ");
            char *cnt = strtok(NULL, " ");
            if (name && cnt) take_item(name, atoi(cnt));
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *name = strtok(NULL, " ");
            if (name) count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free(line);
    free_all();
    return 0;
}