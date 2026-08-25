/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stop {
    char *name;
    char *note;
} Stop;

Stop *stops = NULL;
int stop_count = 0;
int capacity = 0;

void ensure_capacity(int needed) {
    if (needed > capacity) {
        int newcap = capacity ? capacity * 2 : 8;
        while (newcap < needed) newcap *= 2;
        stops = realloc(stops, sizeof(Stop) * newcap);
        capacity = newcap;
    }
}

int find_stop(const char *name) {
    for (int i = 0; i < stop_count; i++) {
        if (strcmp(stops[i].name, name) == 0) return i;
    }
    return -1;
}

void add_stop(const char *name, const char *note) {
    if (find_stop(name) != -1) return;
    ensure_capacity(stop_count + 1);
    stops[stop_count].name = strdup(name);
    stops[stop_count].note = strdup(note);
    stop_count++;
}

void insert_stop(int idx, const char *name, const char *note) {
    if (idx < 0 || idx > stop_count) return;
    if (find_stop(name) != -1) return;
    ensure_capacity(stop_count + 1);
    for (int i = stop_count; i > idx; i--) {
        stops[i] = stops[i-1];
    }
    stops[idx].name = strdup(name);
    stops[idx].note = strdup(note);
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

void remove_stop(int idx) {
    if (idx < 0 || idx >= stop_count) return;
    free(stops[idx].name);
    free(stops[idx].note);
    for (int i = idx; i < stop_count - 1; i++) stops[i] = stops[i+1];
    stop_count--;
}

void find_stop_print(const char *name) {
    int idx = find_stop(name);
    if (idx == -1) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", stops[idx].note);
}

void print_stops(void) {
    for (int i = 0; i < stop_count; i++) {
        printf("%s %s\n", stops[i].name, stops[i].note);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%15s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "ADD") == 0 && n >= 2) {
            char *note = line + strlen(cmd) + strlen(arg1) + 2;
            while (*note == ' ') note++;
            add_stop(arg1, note);
        } else if (strcmp(cmd, "INSERT") == 0 && n >= 3) {
            int idx = atoi(arg1);
            char *note = line + strlen(cmd) + strlen(arg1) + strlen(arg2) + 3;
            while (*note == ' ') note++;
            insert_stop(idx, arg2, note);
        } else if (strcmp(cmd, "MOVE") == 0 && n >= 3) {
            move_stop(atoi(arg1), atoi(arg2));
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) {
            remove_stop(atoi(arg1));
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            find_stop_print(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_stops();
        }
    }
    for (int i = 0; i < stop_count; i++) {
        free(stops[i].name);
        free(stops[i].note);
    }
    free(stops);
    return 0;
}