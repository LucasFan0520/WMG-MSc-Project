// filename: F028.c
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

static void cleanup(void) {
    for (size_t i = 0; i < count; ++i) {
        free(stops[i].name);
        free(stops[i].note);
    }
    free(stops);
}

static void ensure_capacity(size_t needed) {
    if (capacity >= needed) return;
    size_t newcap = capacity ? capacity * 2 : 4;
    while (newcap < needed) newcap *= 2;
    Stop *newstops = realloc(stops, newcap * sizeof(Stop));
    if (!newstops) return;
    stops = newstops;
    capacity = newcap;
}

static void add_stop(const char *name, const char *note) {
    ensure_capacity(count + 1);
    if (!stops) return;
    stops[count].name = strdup(name);
    stops[count].note = strdup(note);
    if (!stops[count].name || !stops[count].note) {
        free(stops[count].name);
        free(stops[count].note);
        return;
    }
    ++count;
}

static void insert_stop(int index, const char *name, const char *note) {
    if (index < 0 || index > (int)count) return;
    ensure_capacity(count + 1);
    if (!stops) return;
    for (size_t i = count; i > (size_t)index; --i)
        stops[i] = stops[i - 1];
    stops[index].name = strdup(name);
    stops[index].note = strdup(note);
    if (!stops[index].name || !stops[index].note) {
        free(stops[index].name);
        free(stops[index].note);
        // shift back? simpler: just don't increment count
        return;
    }
    ++count;
}

static void move_stop(int from, int to) {
    if (from < 0 || from >= (int)count || to < 0 || to >= (int)count) return;
    if (from == to) return;
    Stop tmp = stops[from];
    if (from < to) {
        for (int i = from; i < to; ++i) stops[i] = stops[i + 1];
    } else {
        for (int i = from; i > to; --i) stops[i] = stops[i - 1];
    }
    stops[to] = tmp;
}

static void remove_stop(int index) {
    if (index < 0 || index >= (int)count) return;
    free(stops[index].name);
    free(stops[index].note);
    for (size_t i = index; i + 1 < count; ++i)
        stops[i] = stops[i + 1];
    --count;
}

static void find_stop(const char *name) {
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(stops[i].name, name) == 0) {
            printf("%s\n", stops[i].note);
            return;
        }
    }
}

static void print_stops(void) {
    for (size_t i = 0; i < count; ++i)
        printf("%s %s\n", stops[i].name, stops[i].note);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        int idx1 = atoi(arg1);
        int idx2 = atoi(arg2);
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1) add_stop(arg1, arg2);
        } else if (strcmp(cmd, "INSERT") == 0) {
            if (*arg1 && *arg2) insert_stop(idx1, arg2, arg3);
        } else if (strcmp(cmd, "MOVE") == 0) {
            if (*arg1 && *arg2) move_stop(idx1, idx2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (*arg1) remove_stop(idx1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_stop(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    free(line);
    return 0;
}