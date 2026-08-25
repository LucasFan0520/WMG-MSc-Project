/* F006.c */
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
        i = malloc(sizeof(Item));
        i->name = strdup(name);
        i->count = count;
        i->next = NULL;
        if (!head) head = i;
        else {
            Item *cur = head;
            while (cur->next) cur = cur->next;
            cur->next = i;
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
    Item *i = head, *prev = NULL;
    while (i && strcmp(i->name, name) != 0) {
        prev = i;
        i = i->next;
    }
    if (!i) return;
    if (prev) prev->next = i->next;
    else head = i->next;
    free(i->name);
    free(i);
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
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256];
            int count;
            if (sscanf(line + 4, "%255s %d", name, &count) == 2)
                add_item(name, count);
        } else if (strcmp(cmd, "TAKE") == 0) {
            char name[256];
            int count;
            if (sscanf(line + 5, "%255s %d", name, &count) == 2)
                take_item(name, count);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            remove_item(p);
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            count_item(p);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free_all();
    free(line);
    return 0;
}