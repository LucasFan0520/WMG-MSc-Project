// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char stop[100];
    char note[1000];
} Stop;

Stop stops[1000];
int sn = 0;

int findStop(char *stop) {
    for (int i = 0; i < sn; i++) {
        if (!strcmp(stops[i].stop, stop)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], stop[100], note[1000];
    int idx, from, to;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", stop);
            fgets(note, 1000, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            strcpy(stops[sn].stop, stop);
            strcpy(stops[sn].note, note);
            sn++;
        } else if (!strcmp(cmd, "INSERT")) {
            scanf("%d %s", &idx, stop);
            fgets(note, 1000, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            if (idx >= 0 && idx <= sn) {
                for (int i = sn; i > idx; i--) {
                    stops[i] = stops[i - 1];
                }
                strcpy(stops[idx].stop, stop);
                strcpy(stops[idx].note, note);
                sn++;
            }
        } else if (!strcmp(cmd, "MOVE")) {
            scanf("%d %d", &from, &to);
            if (from >= 0 && from < sn && to >= 0 && to < sn && from != to) {
                Stop tmp = stops[from];
                if (from < to) {
                    for (int i = from; i < to; i++) {
                        stops[i] = stops[i + 1];
                    }
                } else {
                    for (int i = from; i > to; i--) {
                        stops[i] = stops[i - 1];
                    }
                }
                stops[to] = tmp;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%d", &idx);
            if (idx >= 0 && idx < sn) {
                for (int i = idx; i < sn - 1; i++) {
                    stops[i] = stops[i + 1];
                }
                sn--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", stop);
            int i = findStop(stop);
            if (i != -1) {
                printf("%s\n", stops[i].note);
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < sn; i++) {
                printf("%s %s\n", stops[i].stop, stops[i].note);
            }
        }
    }
    return 0;
}