// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct Stop {
    char *name;
    char *note;
} Stop;

Stop *stops = NULL;
int stop_cnt = 0;
int stop_cap = 0;

int find_stop(const char *name) {
    for (int i = 0; i < stop_cnt; i++) {
        if (strcmp(stops[i].name, name) == 0) return i;
    }
    return -1;
}

void add_stop(const char *name, const char *note) {
    if (stop_cnt == stop_cap) {
        stop_cap = stop_cap ? stop_cap * 2 : 4;
        stops = realloc(stops, stop_cap * sizeof(Stop));
    }
    stops[stop_cnt].name = strdup(name);
    stops[stop_cnt].note = strdup(note);
    stop_cnt++;
}

void insert_stop(int idx, const char *name, const char *note) {
    if (idx < 0 || idx > stop_cnt) return;
    if (stop_cnt == stop_cap) {
        stop_cap = stop_cap ? stop_cap * 2 : 4;
        stops = realloc(stops, stop_cap * sizeof(Stop));
    }
    for (int i = stop_cnt; i > idx; i--) {
        stops[i] = stops[i-1];
    }
    stops[idx].name = strdup(name);
    stops[idx].note = strdup(note);
    stop_cnt++;
}

void move_stop(int from, int to) {
    if (from < 0 || from >= stop_cnt || to < 0 || to >= stop_cnt || from == to) return;
    Stop tmp = stops[from];
    if (from < to) {
        for (int i = from; i < to; i++) stops[i] = stops[i+1];
    } else {
        for (int i = from; i > to; i--) stops[i] = stops[i-1];
    }
    stops[to] = tmp;
}

void remove_stop(int idx) {
    if (idx < 0 || idx >= stop_cnt) return;
    free(stops[idx].name);
    free(stops[idx].note);
    for (int i = idx; i < stop_cnt - 1; i++) {
        stops[i] = stops[i+1];
    }
    stop_cnt--;
}

void find_print_stop(const char *name) {
    int idx = find_stop(name);
    if (idx != -1) {
        printf("%s\n", stops[idx].note);
    }
}

void print_stops() {
    for (int i = 0; i < stop_cnt; i++) {
        printf("%s %s\n", stops[i].name, stops[i].note);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0};
        int idx1 = 0, idx2 = 0;
        sscanf(line, "%15s %255s", cmd, arg1);
        
        if (strcmp(cmd, "ADD") == 0) {
            char *note = get_rest(line, 2);
            if (note) add_stop(arg1, note);
        }
        else if (strcmp(cmd, "INSERT") == 0) {
            sscanf(line, "%*s %d %255s", &idx1, arg1);
            char *note = get_rest(line, 3);
            if (note) insert_stop(idx1, arg1, note);
        }
        else if (strcmp(cmd, "MOVE") == 0) {
            sscanf(line, "%*s %d %d", &idx1, &idx2);
            move_stop(idx1, idx2);
        }
        else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %d", &idx1);
            remove_stop(idx1);
        }
        else if (strcmp(cmd, "FIND") == 0) find_print_stop(arg1);
        else if (strcmp(cmd, "PRINT") == 0) print_stops();
        
        free(line);
    }
    for (int i = 0; i < stop_cnt; i++) {
        free(stops[i].name);
        free(stops[i].note);
    }
    free(stops);
    return 0;
}