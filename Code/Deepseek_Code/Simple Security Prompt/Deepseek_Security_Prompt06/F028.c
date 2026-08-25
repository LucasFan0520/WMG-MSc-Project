// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *stop;
    char *note;
    struct Stop *next;
} Stop;

Stop *head = NULL;
Stop *tail = NULL;
int count = 0;

Stop *get_stop(int index) {
    if (index < 0 || index >= count) return NULL;
    Stop *s = head;
    for (int i = 0; i < index; i++) s = s->next;
    return s;
}

void add_stop(const char *stop, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    s->stop = malloc(strlen(stop) + 1);
    strcpy(s->stop, stop);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    s->next = NULL;
    if (tail) tail->next = s;
    else head = s;
    tail = s;
    count++;
}

void insert_stop(int index, const char *stop, const char *note) {
    if (index < 0 || index > count) return;
    Stop *s = malloc(sizeof(Stop));
    s->stop = malloc(strlen(stop) + 1);
    strcpy(s->stop, stop);
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
        if (s->next == NULL) tail = s;
    }
    count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= count || to < 0 || to >= count || from == to) return;
    Stop *prev_from = NULL, *s_from = head;
    for (int i = 0; i < from; i++) { prev_from = s_from; s_from = s_from->next; }
    Stop *prev_to = NULL, *s_to = head;
    for (int i = 0; i < to; i++) { prev_to = s_to; s_to = s_to->next; }
    if (prev_from) prev_from->next = s_from->next;
    else head = s_from->next;
    if (s_from->next == NULL) tail = prev_from;
    if (to == 0) {
        s_from->next = head;
        head = s_from;
        if (!tail) tail = s_from;
    } else {
        s_from->next = prev_to->next;
        prev_to->next = s_from;
        if (s_from->next == NULL) tail = s_from;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= count) return;
    Stop *s = head, *prev = NULL;
    for (int i = 0; i < index; i++) { prev = s; s = s->next; }
    if (prev) prev->next = s->next;
    else head = s->next;
    if (s->next == NULL) tail = prev;
    free(s->stop);
    free(s->note);
    free(s);
    count--;
}

void find_stop(const char *stop) {
    Stop *s = head;
    while (s) {
        if (strcmp(s->stop, stop) == 0) {
            printf("%s\n", s->note);
            return;
        }
        s = s->next;
    }
}

void print_stops(void) {
    Stop *s = head;
    while (s) {
        printf("%s %s\n", s->stop, s->note);
        s = s->next;
    }
}

void free_all(void) {
    Stop *s = head;
    while (s) {
        Stop *tmp = s->next;
        free(s->stop);
        free(s->note);
        free(s);
        s = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        int idx, idx2;
        char stop[256], note[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s", stop) == 1) {
                const char *n = rest + strlen(stop) + 1;
                while (*n == ' ') n++;
                if (*n) add_stop(stop, n);
                else add_stop(stop, "");
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(rest, "%d %255s", &idx, stop) == 2) {
                const char *n = rest;
                while (*n && *n != ' ') n++;
                while (*n == ' ') n++;
                while (*n && *n != ' ') n++;
                while (*n == ' ') n++;
                if (*n) insert_stop(idx, stop, n);
                else insert_stop(idx, stop, "");
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (sscanf(rest, "%d %d", &idx, &idx2) == 2) move_stop(idx, idx2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(rest, "%d", &idx) == 1) remove_stop(idx);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(rest, "%255s", stop) == 1) find_stop(stop);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_all();
    return 0;
}