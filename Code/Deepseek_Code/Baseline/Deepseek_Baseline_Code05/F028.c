/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct stop {
    char *name;
    char *note;
} Stop;

Stop *stops = NULL;
int size = 0, capacity = 0;

void ensure_capacity() {
    if (size >= capacity) {
        capacity = capacity ? capacity * 2 : 4;
        stops = realloc(stops, capacity * sizeof(Stop));
    }
}

int find_stop_index(const char *name) {
    for (int i = 0; i < size; i++) {
        if (strcmp(stops[i].name, name) == 0) return i;
    }
    return -1;
}

void add_stop(const char *name, const char *note) {
    if (find_stop_index(name) != -1) return;
    ensure_capacity();
    stops[size].name = strdup(name);
    stops[size].note = strdup(note);
    size++;
}

void insert_stop(int idx, const char *name, const char *note) {
    if (idx < 0 || idx > size) return;
    if (find_stop_index(name) != -1) return;
    ensure_capacity();
    for (int i = size; i > idx; i--) {
        stops[i] = stops[i-1];
    }
    stops[idx].name = strdup(name);
    stops[idx].note = strdup(note);
    size++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= size || to < 0 || to >= size) return;
    if (from == to) return;
    Stop tmp = stops[from];
    if (from < to) {
        for (int i = from; i < to; i++) stops[i] = stops[i+1];
    } else {
        for (int i = from; i > to; i--) stops[i] = stops[i-1];
    }
    stops[to] = tmp;
}

void remove_stop(int idx) {
    if (idx < 0 || idx >= size) return;
    free(stops[idx].name);
    free(stops[idx].note);
    for (int i = idx; i < size - 1; i++) stops[i] = stops[i+1];
    size--;
}

void find_stop_print(const char *name) {
    int idx = find_stop_index(name);
    if (idx == -1) { printf("NOT_FOUND\n"); return; }
    for (char *c = stops[idx].note; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void print_stops() {
    for (int i = 0; i < size; i++) {
        printf("%s ", stops[i].name);
        for (char *c = stops[i].note; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], note[1024];
        int idx1, idx2;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "PRINT") == 0) { print_stops(); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", name) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            add_stop(name, rest);
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %255s", &idx1, name) < 2) continue;
            char *rest = line + strlen(cmd) + 1 + (idx1 >= 0 ? snprintf(NULL,0,"%d",idx1) : 0) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            insert_stop(idx1, name, rest);
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (sscanf(line, "%*s %d %d", &idx1, &idx2) == 2)
                move_stop(idx1, idx2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %d", &idx1) == 1) remove_stop(idx1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) find_stop_print(name);
        }
    }
    return 0;
}