/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *stop;
    char *note;
} Stop;

Stop *stops = NULL;
int stop_count = 0;
int stop_capacity = 0;

void ensure_stop_capacity(int n) {
    if (n >= stop_capacity) {
        stop_capacity = stop_capacity ? stop_capacity * 2 : 4;
        while (stop_capacity <= n) stop_capacity *= 2;
        stops = realloc(stops, stop_capacity * sizeof(Stop));
    }
}

void add_stop(const char *stop, const char *note) {
    ensure_stop_capacity(stop_count + 1);
    stops[stop_count].stop = strdup(stop);
    stops[stop_count].note = strdup(note);
    stop_count++;
}

void insert_stop(int index, const char *stop, const char *note) {
    if (index < 0 || index > stop_count) return;
    ensure_stop_capacity(stop_count + 1);
    for (int i = stop_count; i > index; i--) {
        stops[i] = stops[i-1];
    }
    stops[index].stop = strdup(stop);
    stops[index].note = strdup(note);
    stop_count++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_count || to < 0 || to >= stop_count) return;
    if (from == to) return;
    Stop tmp = stops[from];
    if (from < to) {
        for (int i = from; i < to; i++) stops[i] = stops[i+1];
    } else {
        for (int i = from; i > to; i--) stops[i] = stops[i-1];
    }
    stops[to] = tmp;
}

void remove_stop(int index) {
    if (index < 0 || index >= stop_count) return;
    free(stops[index].stop);
    free(stops[index].note);
    for (int i = index; i < stop_count - 1; i++) stops[i] = stops[i+1];
    stop_count--;
}

void find_stop(const char *stop) {
    for (int i = 0; i < stop_count; i++) {
        if (strcmp(stops[i].stop, stop) == 0) {
            printf("%s\n", stops[i].note);
            return;
        }
    }
}

void print_stops(void) {
    for (int i = 0; i < stop_count; i++) {
        printf("%s %s\n", stops[i].stop, stops[i].note);
    }
}

void free_stops(void) {
    for (int i = 0; i < stop_count; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    free(stops);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char stop[256], note[4096];
        int idx1, idx2;
        int n = sscanf(line, "%s %d %d %s", cmd, &idx1, &idx2, stop);
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *s = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            if (*p) add_stop(s, p);
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (n < 2) continue;
            char *p = line + strlen(cmd) + 1;
            while (*p == ' ') p++;
            char *end = p;
            while (*end && *end != ' ') end++;
            if (!*end) continue;
            *end = '\0';
            int idx = atoi(p);
            p = end + 1;
            while (*p == ' ') p++;
            char *s = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            if (*p) insert_stop(idx, s, p);
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (n < 3) continue;
            move_stop(idx1, idx2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (n < 2) continue;
            remove_stop(idx1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (n < 2) continue;
            find_stop(stop);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free_stops();
    return 0;
}