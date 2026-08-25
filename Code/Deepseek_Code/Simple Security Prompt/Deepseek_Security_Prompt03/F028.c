/*
 * F028.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stop {
    char *name;
    char *note;
    struct stop *next;
} Stop;

Stop *head = NULL, *tail = NULL;

int count_stops(void) {
    int n = 0;
    Stop *s = head;
    while (s) { n++; s = s->next; }
    return n;
}

Stop *find_stop(const char *name) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_stop(const char *name, const char *note) {
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (tail) tail->next = s;
    else head = s;
    tail = s;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0) return;
    if (find_stop(name)) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    if (index == 0) {
        s->next = head;
        head = s;
        if (!tail) tail = s;
        return;
    }
    Stop *prev = head;
    int i;
    for (i = 0; prev && i < index-1; i++) prev = prev->next;
    if (!prev) { free(s->name); free(s->note); free(s); return; }
    s->next = prev->next;
    prev->next = s;
    if (s->next == NULL) tail = s;
}

void move_stop(int from, int to) {
    int n = count_stops();
    if (from < 0 || from >= n || to < 0 || to >= n || from == to) return;
    Stop *prev_from = NULL, *s_from = head;
    int i;
    for (i = 0; i < from; i++) { prev_from = s_from; s_from = s_from->next; }
    Stop *prev_to = NULL, *s_to = head;
    for (i = 0; i < to; i++) { prev_to = s_to; s_to = s_to->next; }
    if (prev_from) prev_from->next = s_from->next;
    else head = s_from->next;
    if (s_from == tail) tail = prev_from;
    if (to == 0) {
        s_from->next = head;
        head = s_from;
    } else {
        s_from->next = s_to;
        if (prev_to) prev_to->next = s_from;
        else head = s_from;
    }
    if (s_from->next == NULL) tail = s_from;
}

void remove_stop(int index) {
    if (index < 0) return;
    Stop *prev = NULL, *s = head;
    int i;
    for (i = 0; s && i < index; i++) { prev = s; s = s->next; }
    if (!s) return;
    if (prev) prev->next = s->next;
    else head = s->next;
    if (s == tail) tail = prev;
    free(s->name);
    free(s->note);
    free(s);
}

void find_stop_cmd(const char *name) {
    Stop *s = find_stop(name);
    if (!s) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", s->note);
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
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256];
        if (sscanf(line, "%31s %255s %255s", cmd, arg1, arg2) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            add_stop(arg1, rest);
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx = atoi(arg1);
            char *name = rest;
            char *note = name + strlen(arg2) + 1;
            while (*note == ' ') note++;
            insert_stop(idx, name, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from = atoi(arg1);
            int to = atoi(arg2);
            move_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = atoi(arg1);
            remove_stop(idx);
        } else if (strcmp(cmd, "FIND") == 0) {
            find_stop_cmd(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    return 0;
}