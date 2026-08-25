// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

Stop stops[10000];
int sc = 0;

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char stop[1000];
            sscanf(rest, "%s", stop);
            char *note = rest + strlen(stop);
            while (*note == ' ') note++;
            stops[sc].stop = strdup(stop);
            stops[sc].note = strdup(note);
            sc++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char stop[1000];
            sscanf(rest, "%d %s", &idx, stop);
            char *note = rest;
            while (*note && *note != ' ') note++;
            while (*note == ' ') note++;
            while (*note && *note != ' ') note++;
            while (*note == ' ') note++;
            if (idx >= 0 && idx <= sc) {
                for (int i = sc; i > idx; i--) stops[i] = stops[i-1];
                stops[idx].stop = strdup(stop);
                stops[idx].note = strdup(note);
                sc++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            sscanf(rest, "%d %d", &from, &to);
            if (from >= 0 && from < sc && to >= 0 && to < sc) {
                Stop tmp = stops[from];
                if (from < to) {
                    for (int i = from; i < to; i++) stops[i] = stops[i+1];
                } else {
                    for (int i = from; i > to; i--) stops[i] = stops[i-1];
                }
                stops[to] = tmp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx;
            sscanf(rest, "%d", &idx);
            if (idx >= 0 && idx < sc) {
                free(stops[idx].stop);
                free(stops[idx].note);
                for (int i = idx; i < sc - 1; i++) stops[i] = stops[i+1];
                sc--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < sc; i++) {
                if (strcmp(stops[i].stop, rest) == 0) {
                    printf("%s\n", stops[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < sc; i++) {
                printf("%s %s\n", stops[i].stop, stops[i].note);
            }
        }
    }
    for (int i = 0; i < sc; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    return 0;
}