// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
    struct Stop *prev;
} Stop;

Stop *head = NULL, *tail = NULL;
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
    s->prev = tail;
    if (tail) {
        tail->next = s;
        tail = s;
    } else {
        head = tail = s;
    }
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
        s->prev = NULL;
        if (head) head->prev = s;
        head = s;
        if (!tail) tail = s;
    } else if (index == stop_count) {
        s->next = NULL;
        s->prev = tail;
        if (tail) tail->next = s;
        tail = s;
        if (!head) head = s;
    } else {
        Stop *after = get_stop(index);
        if (!after) { free(s->name); free(s->note); free(s); return; }
        Stop *before = after->prev;
        s->prev = before;
        s->next = after;
        before->next = s;
        after->prev = s;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count) return;
    if (from == to) return;
    Stop *s = get_stop(from);
    if (!s) return;
    if (s->prev) s->prev->next = s->next;
    else head = s->next;
    if (s->next) s->next->prev = s->prev;
    else tail = s->prev;
    if (to == 0) {
        s->next = head;
        s->prev = NULL;
        if (head) head->prev = s;
        head = s;
        if (!tail) tail = s;
    } else if (to == stop_count - 1) {
        s->next = NULL;
        s->prev = tail;
        if (tail) tail->next = s;
        tail = s;
        if (!head) head = s;
    } else {
        Stop *after = get_stop(to);
        if (!after) return;
        Stop *before = after->prev;
        s->prev = before;
        s->next = after;
        before->next = s;
        after->prev = s;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *s = get_stop(index);
    if (!s) return;
    if (s->prev) s->prev->next = s->next;
    else head = s->next;
    if (s->next) s->next->prev = s->prev;
    else tail = s->prev;
    free(s->name);
    free(s->note);
    free(s);
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
            char *note = strtok(NULL, "");
            if (name && note) add_stop(name, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx = strtok(NULL, " ");
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (idx && name && note) insert_stop(atoi(idx), name, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from = strtok(NULL, " ");
            char *to = strtok(NULL, " ");
            if (from && to) move_stop(atoi(from), atoi(to));
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx = strtok(NULL, " ");
            if (idx) remove_stop(atoi(idx));
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_stop(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free(line);
    free_all();
    return 0;
}