// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *stop; char *note; } Route;
Route *route = NULL;
int rc = 0, rcap = 0;

void add_stop(const char *stop, const char *note) {
    if (rc == rcap) { rcap = rcap ? rcap * 2 : 4; route = realloc(route, sizeof(Route) * rcap); }
    route[rc].stop = strdup(stop);
    route[rc].note = strdup(note);
    rc++;
}

void insert_stop(int idx, const char *stop, const char *note) {
    if (idx < 0 || idx > rc) return;
    if (rc == rcap) { rcap = rcap ? rcap * 2 : 4; route = realloc(route, sizeof(Route) * rcap); }
    for (int i = rc; i > idx; i--) route[i] = route[i - 1];
    route[idx].stop = strdup(stop);
    route[idx].note = strdup(note);
    rc++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= rc || to < 0 || to >= rc) return;
    if (from == to) return;
    Route tmp = route[from];
    if (from < to) {
        for (int i = from; i < to; i++) route[i] = route[i + 1];
    } else {
        for (int i = from; i > to; i--) route[i] = route[i - 1];
    }
    route[to] = tmp;
}

void remove_stop(int idx) {
    if (idx < 0 || idx >= rc) return;
    free(route[idx].stop);
    free(route[idx].note);
    for (int i = idx; i < rc - 1; i++) route[i] = route[i + 1];
    rc--;
}

void find_stop(const char *stop) {
    for (int i = 0; i < rc; i++)
        if (strcmp(route[i].stop, stop) == 0) { printf("%s\n", route[i].note); return; }
}

void print_route(void) {
    for (int i = 0; i < rc; i++)
        printf("%s %s\n", route[i].stop, route[i].note);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        int idx1, idx2;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                add_stop(arg1, arg2);
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %d %255s %4095[^\n]", &idx1, arg1, arg2) == 3)
                insert_stop(idx1, arg1, arg2);
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (sscanf(line, "%*s %d %d", &idx1, &idx2) == 2)
                move_stop(idx1, idx2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %d", &idx1) == 1) remove_stop(idx1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) find_stop(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) print_route();
    }
    for (int i = 0; i < rc; i++) { free(route[i].stop); free(route[i].note); }
    free(route);
    return 0;
}