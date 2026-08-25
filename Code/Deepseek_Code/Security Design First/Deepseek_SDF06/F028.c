// F028.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
} Stop;

static Stop *stops = NULL;
static size_t count = 0;
static size_t capacity = 0;

static void insert_stop(size_t index, const char *name, const char *note) {
    if (index > count) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Stop *new = realloc(stops, capacity * sizeof(Stop));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        stops = new;
    }
    for (size_t i = count; i > index; i--)
        stops[i] = stops[i - 1];
    stops[index].name = strdup(name);
    stops[index].note = strdup(note);
    if (!stops[index].name || !stops[index].note) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void append_stop(const char *name, const char *note) {
    insert_stop(count, name, note);
}

static void move_stop(size_t from, size_t to) {
    if (from >= count || to >= count) return;
    if (from == to) return;
    Stop tmp = stops[from];
    if (from < to) {
        for (size_t i = from; i < to; i++)
            stops[i] = stops[i + 1];
    } else {
        for (size_t i = from; i > to; i--)
            stops[i] = stops[i - 1];
    }
    stops[to] = tmp;
}

static void remove_stop(size_t index) {
    if (index >= count) return;
    free(stops[index].name);
    free(stops[index].note);
    for (size_t i = index; i < count - 1; i++)
        stops[i] = stops[i + 1];
    count--;
}

static void find_stop(const char *name) {
    for (size_t i = 0; i < count; i++) {
        if (strcmp(stops[i].name, name) == 0) {
            printf("%s\n", stops[i].note);
            return;
        }
    }
}

static void print_stops(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", stops[i].name, stops[i].note);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(stops[i].name);
        free(stops[i].note);
    }
    free(stops);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], note[4096];
    size_t idx1, idx2;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) == 2)
                append_stop(name, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (sscanf(line, "%*s %zu %255s %4095[^\n]", &idx1, name, note) == 3)
                insert_stop(idx1, name, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (sscanf(line, "%*s %zu %zu", &idx1, &idx2) == 2)
                move_stop(idx1, idx2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %zu", &idx1) == 1)
                remove_stop(idx1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_stop(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }

    free(line);
    free_all();
    return 0;
}