// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stop {
    char *name;
    char *note;
    struct stop *next;
} Stop;

Stop *head = NULL;
Stop *tail = NULL;
int stop_count = 0;

Stop *get_stop(int index) {
    if (index < 0) return NULL;
    Stop *s = head;
    for (int i = 0; i < index && s; i++) s = s->next;
    return s;
}

void add_stop(const char *name, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (tail) tail->next = s;
    else head = s;
    tail = s;
    stop_count++;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > stop_count) return;
    Stop *new = malloc(sizeof(Stop));
    new->name = strdup(name);
    new->note = strdup(note);
    if (index == 0) {
        new->next = head;
        head = new;
        if (!tail) tail = new;
    } else {
        Stop *prev = get_stop(index - 1);
        new->next = prev->next;
        prev->next = new;
        if (!new->next) tail = new;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || to < 0 || from >= stop_count || to >= stop_count) return;
    if (from == to) return;
    Stop *prev_from = NULL, *prev_to = NULL;
    Stop *s = head;
    for (int i = 0; i < stop_count; i++) {
        if (i == from - 1) prev_from = s;
        if (i == to - 1) prev_to = s;
        if (i == from) break;
        s = s->next;
    }
    Stop *f = get_stop(from);
    Stop *t = get_stop(to);
    if (!f || !t) return;
    // remove from from
    if (prev_from) prev_from->next = f->next;
    else head = f->next;
    if (f == tail) tail = prev_from;
    // insert at to
    if (to == 0) {
        f->next = head;
        head = f;
    } else {
        Stop *prev = get_stop(to - 1);
        f->next = prev->next;
        prev->next = f;
    }
    if (!f->next) tail = f;
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    if (index == 0) {
        Stop *tmp = head;
        head = tmp->next;
        if (!head) tail = NULL;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    } else {
        Stop *prev = get_stop(index - 1);
        Stop *tmp = prev->next;
        prev->next = tmp->next;
        if (!prev->next) tail = prev;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
    stop_count--;
}

void find_stop(const char *name) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            printf("%s\n", s->note);
            return;
        }
        s = s->next;
    }
}

void print_stops(void) {
    Stop *s = head;
    while (s) {
        printf("%s %s\n", s->name, s->note);
        s = s->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        int idx1, idx2;
        char name[256], note[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", name, note) == 2)
                add_stop(name, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %255s %[^\n]", &idx1, name, note) == 3)
                insert_stop(idx1, name, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (sscanf(line, "%*s %d %d", &idx1, &idx2) == 2)
                move_stop(idx1, idx2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %d", &idx1) == 1)
                remove_stop(idx1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_stop(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    return 0;
}