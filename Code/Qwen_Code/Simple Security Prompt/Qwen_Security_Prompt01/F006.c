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

void add_item(const char *name, int c) {
    for (Item *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            p->count += c;
            return;
        }
    }
    Item *p = malloc(sizeof(Item));
    p->name = strdup(name);
    p->count = c;
    p->next = NULL;
    if (!head) {
        head = tail = p;
    } else {
        tail->next = p;
        tail = p;
    }
}

void take_item(const char *name, int c) {
    for (Item *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            p->count -= c;
            if (p->count < 0) {
                p->count = 0;
            }
            return;
        }
    }
}

void remove_item(const char *name) {
    Item *p = head;
    Item *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            Item *next = p->next;
            free(p->name);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            if (tail == p) {
                tail = prev;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void count_item(const char *name) {
    for (Item *p = head; p; p = p->next) {
        if (strcmp(p->name, name) == 0) {
            printf("%d\n", p->count);
            return;
        }
    }
    printf("0\n");
}

void report_items() {
    for (Item *p = head; p; p = p->next) {
        printf("%s %d\n", p->name, p->count);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char name[1000];
        int c;
        sscanf(line, "%s %s", cmd, name);
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %*s %d", &c);
            add_item(name, c);
        } else if (strcmp(cmd, "TAKE") == 0) {
            sscanf(line, "%*s %*s %d", &c);
            take_item(name, c);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_item(name);
        } else if (strcmp(cmd, "COUNT") == 0) {
            count_item(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    while (head) {
        Item *n = head->next;
        free(head->name);
        free(head);
        head = n;
    }
    return 0;
}