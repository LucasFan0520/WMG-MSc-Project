// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_STOP 64
#define MAX_NOTE 4096

typedef struct Stop {
    char name[MAX_STOP];
    char note[MAX_NOTE];
    struct Stop *next;
} Stop;

Stop *head = NULL;
int stop_count = 0;

Stop *get_stop(int index) {
    if (index < 0 || index >= stop_count) return NULL;
    Stop *s = head;
    for (int i = 0; i < index; i++) s = s->next;
    return s;
}

void free_all() {
    Stop *s = head;
    while (s) {
        Stop *tmp = s;
        s = s->next;
        free(tmp);
    }
    head = NULL;
    stop_count = 0;
}

void handle_add(char *line) {
    char name[MAX_STOP], note[MAX_NOTE];
    int n = sscanf(line, "ADD %63s %4095[^\n]", name, note);
    if (n < 2) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    strcpy(s->name, name);
    strcpy(s->note, note);
    s->next = NULL;
    if (!head) {
        head = s;
    } else {
        Stop *last = head;
        while (last->next) last = last->next;
        last->next = s;
    }
    stop_count++;
}

void handle_insert(char *line) {
    int index;
    char name[MAX_STOP], note[MAX_NOTE];
    int n = sscanf(line, "INSERT %d %63s %4095[^\n]", &index, name, note);
    if (n < 3) return;
    if (index < 0 || index > stop_count) return;
    Stop *s = malloc(sizeof(Stop));
    if (!s) return;
    strcpy(s->name, name);
    strcpy(s->note, note);
    if (index == 0) {
        s->next = head;
        head = s;
    } else {
        Stop *prev = get_stop(index - 1);
        s->next = prev->next;
        prev->next = s;
    }
    stop_count++;
}

void handle_move(char *line) {
    int from, to;
    if (sscanf(line, "MOVE %d %d", &from, &to) != 2) return;
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count) return;
    if (from == to) return;
    Stop *prev_from = NULL, *node_from = head;
    for (int i = 0; i < from; i++) { prev_from = node_from; node_from = node_from->next; }
    Stop *prev_to = NULL, *node_to = head;
    for (int i = 0; i < to; i++) { prev_to = node_to; node_to = node_to->next; }
    if (prev_from) prev_from->next = node_from->next;
    else head = node_from->next;
    if (from < to) {
        node_from->next = node_to;
        if (prev_to) prev_to->next = node_from;
        else head = node_from;
    } else {
        if (prev_to) prev_to->next = node_from;
        else head = node_from;
        node_from->next = node_to;
    }
}

void handle_remove(char *line) {
    int index;
    if (sscanf(line, "REMOVE %d", &index) != 1) return;
    if (index < 0 || index >= stop_count) return;
    Stop *to_del;
    if (index == 0) {
        to_del = head;
        head = head->next;
    } else {
        Stop *prev = get_stop(index - 1);
        to_del = prev->next;
        prev->next = to_del->next;
    }
    free(to_del);
    stop_count--;
}

void handle_find(char *line) {
    char name[MAX_STOP];
    if (sscanf(line, "FIND %63s", name) != 1) return;
    Stop *s = head;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            printf("%s\n", s->note);
            return;
        }
        s = s->next;
    }
}

void handle_print() {
    Stop *s = head;
    while (s) {
        printf("%s %s\n", s->name, s->note);
        s = s->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "INSERT ", 7) == 0) handle_insert(line);
        else if (strncmp(line, "MOVE ", 5) == 0) handle_move(line);
        else if (strncmp(line, "REMOVE ", 7) == 0) handle_remove(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "PRINT") == 0) handle_print();
    }
    free_all();
    return 0;
}