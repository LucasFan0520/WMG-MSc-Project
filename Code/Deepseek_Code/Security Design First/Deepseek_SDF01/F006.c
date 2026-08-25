// filename: F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ItemNode {
    char *name;
    int count;
    struct ItemNode *next;
} ItemNode;

static ItemNode *head = NULL;

static void cleanup(void) {
    while (head) {
        ItemNode *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp);
    }
}

static ItemNode *find_item(const char *name) {
    for (ItemNode *n = head; n; n = n->next)
        if (strcmp(n->name, name) == 0)
            return n;
    return NULL;
}

static void add_item(const char *name, int count) {
    if (count < 0) count = 0;
    ItemNode *exist = find_item(name);
    if (exist) {
        exist->count += count;
        return;
    }
    ItemNode *n = malloc(sizeof(ItemNode));
    if (!n) return;
    n->name = strdup(name);
    if (!n->name) { free(n); return; }
    n->count = count;
    n->next = head;
    head = n;
}

static void take_item(const char *name, int count) {
    ItemNode *n = find_item(name);
    if (!n) return;
    if (count < 0) count = 0;
    if (n->count > count) n->count -= count;
    else n->count = 0;
}

static void remove_item(const char *name) {
    ItemNode *prev = NULL;
    for (ItemNode *n = head; n; n = n->next) {
        if (strcmp(n->name, name) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free(n->name);
            free(n);
            return;
        }
        prev = n;
    }
}

static void count_item(const char *name) {
    ItemNode *n = find_item(name);
    printf("%d\n", n ? n->count : 0);
}

static void report_items(void) {
    for (ItemNode *n = head; n; n = n->next)
        printf("%s %d\n", n->name, n->count);
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        int cnt = atoi(arg2);
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1) add_item(arg1, cnt);
        } else if (strcmp(cmd, "TAKE") == 0) {
            if (*arg1) take_item(arg1, cnt);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (*arg1) remove_item(arg1);
        } else if (strcmp(cmd, "COUNT") == 0) {
            if (*arg1) count_item(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_items();
        }
    }
    free(line);
    return 0;
}