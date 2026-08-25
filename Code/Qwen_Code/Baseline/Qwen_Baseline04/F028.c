// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

Stop *stops = NULL;
int scount = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char stop[100], note[1000];
            scanf("%s", stop);
            scanf(" %[^\n]", note);
            stops = realloc(stops, sizeof(Stop) * (scount + 1));
            stops[scount].stop = strdup(stop);
            stops[scount].note = strdup(note);
            scount++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            char stop[100], note[1000];
            scanf("%d %s", &idx, stop);
            scanf(" %[^\n]", note);
            if (idx >= 0 && idx <= scount) {
                stops = realloc(stops, sizeof(Stop) * (scount + 1));
                for (int i = scount; i > idx; i--) stops[i] = stops[i-1];
                stops[idx].stop = strdup(stop);
                stops[idx].note = strdup(note);
                scount++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            scanf("%d %d", &from, &to);
            if (from >= 0 && from < scount && to >= 0 && to < scount && from != to) {
                Stop temp = stops[from];
                if (from < to) {
                    for (int i = from; i < to; i++) stops[i] = stops[i+1];
                } else {
                    for (int i = from; i > to; i--) stops[i] = stops[i-1];
                }
                stops[to] = temp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx;
            scanf("%d", &idx);
            if (idx >= 0 && idx < scount) {
                free(stops[idx].stop);
                free(stops[idx].note);
                for (int i = idx; i < scount - 1; i++) stops[i] = stops[i+1];
                scount--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char stop[100];
            scanf("%s", stop);
            for (int i = 0; i < scount; i++) {
                if (strcmp(stops[i].stop, stop) == 0) {
                    printf("%s\n", stops[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < scount; i++) {
                printf("%s %s\n", stops[i].stop, stops[i].note);
            }
        }
    }
    for (int i = 0; i < scount; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    free(stops);
    return 0;
}