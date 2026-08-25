// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stop {
    char *stop;
    char *note;
    struct stop *next;
};

struct stop *stops = NULL;
struct stop *tail = NULL;
int stop_count = 0;

struct stop *find_stop(char *stop_name) {
    struct stop *s = stops;
    while (s) {
        if (strcmp(s->stop, stop_name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_stop(char *stop_name, char *note) {
    if (find_stop(stop_name)) return;
    struct stop *s = malloc(sizeof(struct stop));
    s->stop = malloc(strlen(stop_name) + 1);
    strcpy(s->stop, stop_name);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    s->next = NULL;
    if (tail) {
        tail->next = s;
        tail = s;
    } else {
        stops = tail = s;
    }
    stop_count++;
}

void insert_stop(int index, char *stop_name, char *note) {
    if (index < 0 || index > stop_count) return;
    if (find_stop(stop_name)) return;
    struct stop *s = malloc(sizeof(struct stop));
    s->stop = malloc(strlen(stop_name) + 1);
    strcpy(s->stop, stop_name);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    if (index == 0) {
        s->next = stops;
        stops = s;
        if (!tail) tail = s;
    } else {
        struct stop *prev = stops;
        for (int i = 0; i < index - 1; i++) prev = prev->next;
        s->next = prev->next;
        prev->next = s;
        if (s->next == NULL) tail = s;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count || from == to) return;
    // remove from from
    struct stop *prev_from = NULL, *cur = stops;
    for (int i = 0; i < from; i++) { prev_from = cur; cur = cur->next; }
    struct stop *moving = cur;
    if (prev_from) prev_from->next = cur->next;
    else stops = cur->next;
    if (cur == tail) tail = prev_from;
    stop_count--;
    // insert at to
    if (to == 0) {
        moving->next = stops;
        stops = moving;
        if (!tail) tail = moving;
    } else {
        struct stop *prev_to = stops;
        for (int i = 0; i < to - 1; i++) prev_to = prev_to->next;
        moving->next = prev_to->next;
        prev_to->next = moving;
        if (moving->next == NULL) tail = moving;
    }
    stop_count++;
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    struct stop *cur = stops, *prev = NULL;
    for (int i = 0; i < index; i++) { prev = cur; cur = cur->next; }
    if (prev) prev->next = cur->next;
    else stops = cur->next;
    if (cur == tail) tail = prev;
    free(cur->stop);
    free(cur->note);
    free(cur);
    stop_count--;
}

void find_stop_print(char *stop_name) {
    struct stop *s = find_stop(stop_name);
    if (s) printf("%s\n", s->note);
}

void print_stops() {
    struct stop *s = stops;
    while (s) {
        printf("%s %s\n", s->stop, s->note);
        s = s->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        int index, from, to;
        int n = sscanf(line, "%31s %d %d %255s %4095[^\n]", cmd, &from, &to, arg1, arg2);
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%31s %255s %4095[^\n]", cmd, arg1, arg2) >= 2) {
                char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1;
                add_stop(arg1, rest);
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%31s %d %255s %4095[^\n]", cmd, &index, arg1, arg2) >= 3) {
                char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1;
                insert_stop(index, arg1, rest);
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (sscanf(line, "%31s %d %d", cmd, &from, &to) >= 3) {
                move_stop(from, to);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%31s %d", cmd, &index) >= 2) {
                remove_stop(index);
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%31s %255s", cmd, arg1) >= 2) {
                find_stop_print(arg1);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    return 0;
}