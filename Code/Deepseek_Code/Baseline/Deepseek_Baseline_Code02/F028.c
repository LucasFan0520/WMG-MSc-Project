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
    Stop *s = head;
    for (int i = 0; i < index; i++) s = s->next;
    return s;
}

void add_stop(const char *name, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    s->name = malloc(strlen(name) + 1);
    strcpy(s->name, name);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    s->next = NULL;
    if (tail) tail->next = s;
    else head = s;
    tail = s;
    stop_count++;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > stop_count) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = malloc(strlen(name) + 1);
    strcpy(s->name, name);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    if (index == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else {
        Stop *prev = get_stop(index - 1);
        s->next = prev->next;
        prev->next = s;
        if (!s->next) tail = s;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count || from == to) return;
    Stop *prev_from = NULL;
    Stop *s = head;
    for (int i = 0; i < from; i++) { prev_from = s; s = s->next; }
    // remove from
    if (prev_from) prev_from->next = s->next;
    else head = s->next;
    if (tail == s) tail = prev_from;
    // insert at to
    if (to == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
    } else {
        Stop *prev_to = head;
        for (int i = 0; i < to - 1; i++) prev_to = prev_to->next;
        s->next = prev_to->next;
        prev_to->next = s;
        if (!s->next) tail = s;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *prev = NULL;
    Stop *s = head;
    for (int i = 0; i < index; i++) { prev = s; s = s->next; }
    if (prev) prev->next = s->next;
    else head = s->next;
    if (tail == s) tail = prev;
    free(s->name);
    free(s->note);
    free(s);
    stop_count--;
}

void find_stop(const char *name) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            for (char *c = s->note; *c; c++) {
                putchar(*c == ' ' ? '_' : *c);
            }
            putchar('\n');
            return;
        }
        s = s->next;
    }
}

void print_stops(void) {
    Stop *s = head;
    while (s) {
        printf("%s ", s->name);
        for (char *c = s->note; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        s = s->next;
    }
}

void free_all(void) {
    Stop *s = head;
    while (s) {
        Stop *next = s->next;
        free(s->name);
        free(s->note);
        free(s);
        s = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_stop(stop, p);
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            int idx;
            if (sscanf(p, "%d", &idx) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            char *stop = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            insert_stop(idx, stop, p);
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            int from, to;
            if (sscanf(p, "%d %d", &from, &to) != 2) continue;
            move_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            int idx;
            if (sscanf(p, "%d", &idx) == 1) remove_stop(idx);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            find_stop(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_all();
    return 0;
}