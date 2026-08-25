// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

Stop *stops = NULL;
int num_stops = 0;

int find_stop(const char *name) {
    for (int i = 0; i < num_stops; i++) {
        if (strcmp(stops[i].stop, name) == 0) return i;
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64];
        sscanf(line, "%s", cmd);
        
        if (strcmp(cmd, "ADD") == 0) {
            char stop[256];
            sscanf(line, "%*s %s", stop);
            char *note = strstr(line, stop);
            if (note) {
                note += strlen(stop);
                if (*note == ' ') note++;
            } else {
                note = "";
            }
            stops = realloc(stops, (num_stops + 1) * sizeof(Stop));
            stops[num_stops].stop = strdup(stop);
            stops[num_stops].note = strdup(note);
            num_stops++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char stop[256];
            sscanf(line, "%*s %d %s", &idx, stop);
            char *note = strstr(line, stop);
            if (note) {
                note += strlen(stop);
                if (*note == ' ') note++;
            } else {
                note = "";
            }
            if (idx >= 0 && idx <= num_stops) {
                stops = realloc(stops, (num_stops + 1) * sizeof(Stop));
                for (int i = num_stops; i > idx; i--) {
                    stops[i] = stops[i - 1];
                }
                stops[idx].stop = strdup(stop);
                stops[idx].note = strdup(note);
                num_stops++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            sscanf(line, "%*s %d %d", &from, &to);
            if (from >= 0 && from < num_stops && to >= 0 && to < num_stops && from != to) {
                Stop tmp = stops[from];
                if (from < to) {
                    for (int i = from; i < to; i++) stops[i] = stops[i + 1];
                } else {
                    for (int i = from; i > to; i--) stops[i] = stops[i - 1];
                }
                stops[to] = tmp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx;
            sscanf(line, "%*s %d", &idx);
            if (idx >= 0 && idx < num_stops) {
                free(stops[idx].stop);
                free(stops[idx].note);
                for (int i = idx; i < num_stops - 1; i++) {
                    stops[i] = stops[i + 1];
                }
                num_stops--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char stop[256];
            sscanf(line, "%*s %s", stop);
            int idx = find_stop(stop);
            if (idx >= 0) {
                printf("%s\n", stops[idx].note);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < num_stops; i++) {
                printf("%s %s\n", stops[i].stop, stops[i].note);
            }
        }
    }
    
    for (int i = 0; i < num_stops; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    free(stops);
    
    return 0;
}