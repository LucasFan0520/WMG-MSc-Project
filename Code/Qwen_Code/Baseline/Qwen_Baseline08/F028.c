// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STOPS 10000
#define MAX_LEN 1000

typedef struct {
    char stop[100];
    char note[MAX_LEN];
} Stop;

Stop stops[MAX_STOPS];
int scount = 0;

int find_stop(const char *stop) {
    for (int i = 0; i < scount; i++) if (strcmp(stops[i].stop, stop) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], stop[100], note[MAX_LEN];
    int idx, from, to;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", stop);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (scount < MAX_STOPS) {
                strcpy(stops[scount].stop, stop);
                strcpy(stops[scount].note, note);
                scount++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            scanf("%d %s", &idx, stop);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (idx >= 0 && idx <= scount && scount < MAX_STOPS) {
                for (int i = scount; i > idx; i--) stops[i] = stops[i - 1];
                strcpy(stops[idx].stop, stop);
                strcpy(stops[idx].note, note);
                scount++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            scanf("%d %d", &from, &to);
            if (from >= 0 && from < scount && to >= 0 && to < scount && from != to) {
                Stop temp = stops[from];
                if (from < to) {
                    for (int i = from; i < to; i++) stops[i] = stops[i + 1];
                } else {
                    for (int i = from; i > to; i--) stops[i] = stops[i - 1];
                }
                stops[to] = temp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%d", &idx);
            if (idx >= 0 && idx < scount) {
                for (int i = idx; i < scount - 1; i++) stops[i] = stops[i + 1];
                scount--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", stop);
            int idx = find_stop(stop);
            if (idx != -1) printf("%s\n", stops[idx].note);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < scount; i++) {
                printf("%s %s\n", stops[i].stop, stops[i].note);
            }
        }
    }
    return 0;
}