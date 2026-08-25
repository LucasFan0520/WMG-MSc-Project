// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
    struct Stop *next;
} Stop;

Stop *stops = NULL;
int stop_count = 0;

Stop *get_stop(int index) {
    if (index < 0 || index >= stop_count) return NULL;
    Stop *s = stops;
    for (int i = 0; i < index; i++) s = s->next;
    return s;
}

void add_stop(const char *name, const char *note) {
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = NULL;
    if (!stops) {
        stops = s;
    } else {
        Stop *curr = stops;
        while (curr->next) curr = curr->next;
        curr->next = s;
    }
    stop_count++;
}

void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > stop_count) return;
    Stop *s = malloc(sizeof(Stop));
    s->name = strdup(name);
    s->note = strdup(note);
    if (index == 0) {
        s->next = stops;
        stops = s;
    } else {
        Stop *prev = get_stop(index - 1);
        s->next = prev->next;
        prev->next = s;
    }
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count || from == to) return;
    Stop *prev_from = NULL;
    Stop *curr_from = stops;
    for (int i = 0; i < from; i++) {
        prev_from = curr_from;
        curr_from = curr_from->next;
    }
    if (prev_from) prev_from->next = curr_from->next;
    else stops = curr_from->next;

    if (to == 0) {
        curr_from->next = stops;
        stops = curr_from;
    } else {
        Stop *prev_to = stops;
        for (int i = 0; i < to - 1; i++) prev_to = prev_to->next;
        curr_from->next = prev_to->next;
        prev_to->next = curr_from;
    }
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    Stop *prev = NULL;
    Stop *curr = stops;
    for (int i = 0; i < index; i++) {
        prev = curr;
        curr = curr->next;
    }
    if (prev) prev->next = curr->next;
    else stops = curr->next;
    free(curr->name);
    free(curr->note);
    free(curr);
    stop_count--;
}

void find_stop(const char *name) {
    for (Stop *s = stops; s; s = s->next) {
        if (strcmp(s->name, name) == 0) {
            printf("%s\n", s->note);
            return;
        }
    }
}

void print_stops() {
    for (Stop *s = stops; s; s = s->next) {
        printf("%s %s\n", s->name, s->note);
    }
}

void cleanup() {
    while (stops) {
        Stop *tmp = stops->next;
        free(stops->name);
        free(stops->note);
        free(stops);
        stops = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256];
        int idx1, idx2;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) add_stop(a1, t + 1); }
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %255s", &idx1, a1) == 2) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) { t = strchr(t + 1, ' '); if (t) insert_stop(idx1, a1, t + 1); } }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (sscanf(line, "%*s %d %d", &idx1, &idx2) == 2) move_stop(idx1, idx2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %d", &idx1) == 1) remove_stop(idx1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) find_stop(a1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    cleanup();
    return 0;
}