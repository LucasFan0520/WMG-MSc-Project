// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;
Stop *tail = NULL;
int stop_count = 0;

Stop *get_stop(int index) {
    if (index < 0 || index >= stop_count) return NULL;
    Stop *cur = head;
    for (int i = 0; i < index; i++) cur = cur->next;
    return cur;
}

void add_stop(const char *name, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (!head) head = tail = s;
    else {
        tail->next = s;
        tail = s;
    }
    stop_count++;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > stop_count) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    if (index == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else {
        Stop *prev = get_stop(index - 1);
        s->next = prev->next;
        prev->next = s;
        if (s->next == NULL) tail = s;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count || from == to) return;
    Stop *f = get_stop(from);
    // remove from
    Stop *cur = head, *prev = NULL;
    while (cur) {
        if (cur == f) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            if (cur == tail) tail = prev;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    stop_count--;
    // insert at to
    if (to > stop_count) to = stop_count;
    if (to == 0) {
        f->next = head;
        head = f;
        if (!tail) tail = f;
    } else {
        Stop *p = get_stop(to - 1);
        f->next = p->next;
        p->next = f;
        if (f->next == NULL) tail = f;
    }
    stop_count++;
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *cur = head, *prev = NULL;
    for (int i = 0; i < index; i++) { prev = cur; cur = cur->next; }
    if (prev) prev->next = cur->next;
    else head = cur->next;
    if (cur == tail) tail = prev;
    free(cur->name);
    free(cur->note);
    free(cur);
    stop_count--;
}

void find_stop(const char *name) {
    Stop *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            printf("%s\n", cur->note);
            return;
        }
        cur = cur->next;
    }
}

void print_stops(void) {
    Stop *cur = head;
    while (cur) {
        printf("%s %s\n", cur->name, cur->note);
        cur = cur->next;
    }
}

void cleanup_stops(void) {
    Stop *cur = head;
    while (cur) {
        Stop *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], rest[2048];
        int n = sscanf(line, "%31s %255s %255s %2047s", cmd, arg1, arg2, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *note = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            add_stop(arg1, note);
        } else if (strcmp(cmd, "INSERT") == 0 && n >= 3) {
            int index = atoi(arg1);
            char *note = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(arg2) + 1;
            insert_stop(index, arg2, note);
        } else if (strcmp(cmd, "MOVE") == 0 && n >= 3) {
            int from = atoi(arg1);
            int to = atoi(arg2);
            move_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int index = atoi(arg1);
            remove_stop(index);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_stop(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    cleanup_stops();
    return 0;
}