/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
} Stop;

static Stop *stops = NULL;
static size_t scount = 0, scap = 0;

static void insert_stop(size_t idx, const char *name, const char *note) {
    if (idx > scount) return;
    if (scount == scap) {
        scap = scap ? scap * 2 : 4;
        stops = realloc(stops, scap * sizeof(Stop));
        if (!stops) { perror("realloc"); exit(1); }
    }
    for (size_t i = scount; i > idx; i--)
        stops[i] = stops[i - 1];
    stops[idx].name = strdup(name);
    stops[idx].note = strdup(note);
    scount++;
}

static void append_stop(const char *name, const char *note) {
    insert_stop(scount, name, note);
}

static void move_stop(size_t from, size_t to) {
    if (from >= scount || to >= scount) return;
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

static void remove_stop(size_t idx) {
    if (idx >= scount) return;
    free(stops[idx].name);
    free(stops[idx].note);
    for (size_t i = idx; i < scount - 1; i++)
        stops[i] = stops[i + 1];
    scount--;
}

static void find_stop(const char *name) {
    for (size_t i = 0; i < scount; i++) {
        if (strcmp(stops[i].name, name) == 0) {
            printf("%s\n", stops[i].note);
            return;
        }
    }
}

static void print_stops(void) {
    for (size_t i = 0; i < scount; i++) {
        printf("%s %s\n", stops[i].name, stops[i].note);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            append_stop(name, note);
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idxstr = strtok(NULL, " ");
            if (!idxstr) continue;
            int idx = atoi(idxstr);
            if (idx < 0) continue;
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            insert_stop((size_t)idx, name, note);
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from = strtok(NULL, " ");
            char *to = strtok(NULL, " ");
            if (!from || !to) continue;
            int fi = atoi(from), ti = atoi(to);
            if (fi < 0 || ti < 0) continue;
            move_stop((size_t)fi, (size_t)ti);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idxstr = strtok(NULL, " ");
            if (!idxstr) continue;
            int idx = atoi(idxstr);
            if (idx < 0) continue;
            remove_stop((size_t)idx);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) find_stop(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    for (size_t i = 0; i < scount; i++) {
        free(stops[i].name);
        free(stops[i].note);
    }
    free(stops);
    return 0;
}