// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *stop; char *note; } Stop;
Stop *stops = NULL;
size_t sc = 0, scap = 0;

void add_stop(const char *stop, const char *note) {
    if (sc == scap) {
        size_t nc = scap ? scap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Stop)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Stop *tmp = realloc(stops, nc * sizeof(Stop));
        if (!tmp) { perror("realloc"); exit(1); }
        stops = tmp;
        scap = nc;
    }
    stops[sc].stop = strdup(stop);
    stops[sc].note = strdup(note);
    if (!stops[sc].stop || !stops[sc].note) { perror("strdup"); exit(1); }
    sc++;
}

void ins_stop(size_t idx, const char *stop, const char *note) {
    if (idx > sc) return;
    if (sc == scap) {
        size_t nc = scap ? scap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Stop)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Stop *tmp = realloc(stops, nc * sizeof(Stop));
        if (!tmp) { perror("realloc"); exit(1); }
        stops = tmp;
        scap = nc;
    }
    for (size_t i = sc; i > idx; i--) stops[i] = stops[i-1];
    stops[idx].stop = strdup(stop);
    stops[idx].note = strdup(note);
    if (!stops[idx].stop || !stops[idx].note) { perror("strdup"); exit(1); }
    sc++;
}

void mov_stop(size_t from, size_t to) {
    if (from >= sc || to >= sc || from == to) return;
    Stop tmp = stops[from];
    if (from < to) {
        for (size_t i = from; i < to; i++) stops[i] = stops[i+1];
    } else {
        for (size_t i = from; i > to; i--) stops[i] = stops[i-1];
    }
    stops[to] = tmp;
}

void rem_stop(size_t idx) {
    if (idx >= sc) return;
    free(stops[idx].stop); free(stops[idx].note);
    for (size_t i = idx; i < sc - 1; i++) stops[i] = stops[i+1];
    sc--;
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char stop[256];
            if (sscanf(line, "%*s %255s", stop) != 1) { free(line); continue; }
            char *note = strchr(line, ' ');
            if (note) { note = strchr(note + 1, ' '); if (note) note++; }
            if (!note) note = "";
            add_stop(stop, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            size_t idx; char stop[256];
            if (sscanf(line, "%*s %zu %255s", &idx, stop) == 2) {
                char *note = strchr(line, ' ');
                if (note) { note = strchr(note + 1, ' '); if (note) note = strchr(note + 1, ' '); if (note) note++; }
                if (!note) note = "";
                ins_stop(idx, stop, note);
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            size_t from, to;
            if (sscanf(line, "%*s %zu %zu", &from, &to) == 2) mov_stop(from, to);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            size_t idx;
            if (sscanf(line, "%*s %zu", &idx) == 1) rem_stop(idx);
        } else if (strcmp(cmd, "FIND") == 0) {
            char stop[256];
            if (sscanf(line, "%*s %255s", stop) == 1) {
                for (size_t i = 0; i < sc; i++) {
                    if (strcmp(stops[i].stop, stop) == 0) {
                        printf("%s\n", stops[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < sc; i++) printf("%s %s\n", stops[i].stop, stops[i].note);
        }
        free(line);
    }
    for (size_t i = 0; i < sc; i++) { free(stops[i].stop); free(stops[i].note); }
    free(stops);
    return 0;
}