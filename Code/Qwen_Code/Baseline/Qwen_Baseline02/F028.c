// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *stop; char *note; } Stop;
Stop stops[10000]; int nstops = 0;

int find_stop(char *stop) {
    for (int i = 0; i < nstops; i++) if (!strcmp(stops[i].stop, stop)) return i;
    return -1;
}

int main() {
    char cmd[20], stop[100], note[1000]; int idx1, idx2;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            fgets(stop, sizeof(stop), stdin);
            stop[strcspn(stop, "\n")] = 0;
            if (stop[0] == ' ') memmove(stop, stop + 1, strlen(stop));
            char *space = strchr(stop, ' ');
            if (space) {
                *space = 0;
                char *n = space + 1;
                while(*n == ' ') n++;
                stops[nstops].stop = strdup(stop);
                stops[nstops++].note = strdup(n);
            }
        } else if (!strcmp(cmd, "INSERT")) {
            scanf("%d", &idx1);
            fgets(stop, sizeof(stop), stdin);
            stop[strcspn(stop, "\n")] = 0;
            if (stop[0] == ' ') memmove(stop, stop + 1, strlen(stop));
            char *space = strchr(stop, ' ');
            if (space && idx1 >= 0 && idx1 <= nstops) {
                *space = 0;
                char *n = space + 1;
                while(*n == ' ') n++;
                for (int i = nstops; i > idx1; i--) stops[i] = stops[i - 1];
                stops[idx1].stop = strdup(stop);
                stops[idx1].note = strdup(n);
                nstops++;
            }
        } else if (!strcmp(cmd, "MOVE")) {
            scanf("%d %d", &idx1, &idx2);
            if (idx1 >= 0 && idx1 < nstops && idx2 >= 0 && idx2 < nstops && idx1 != idx2) {
                Stop t = stops[idx1];
                if (idx1 < idx2) {
                    for (int i = idx1; i < idx2; i++) stops[i] = stops[i + 1];
                } else {
                    for (int i = idx1; i > idx2; i--) stops[i] = stops[i - 1];
                }
                stops[idx2] = t;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%d", &idx1);
            if (idx1 >= 0 && idx1 < nstops) {
                free(stops[idx1].stop);
                free(stops[idx1].note);
                for (int i = idx1; i < nstops - 1; i++) stops[i] = stops[i + 1];
                nstops--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            fgets(stop, sizeof(stop), stdin);
            stop[strcspn(stop, "\n")] = 0;
            if (stop[0] == ' ') memmove(stop, stop + 1, strlen(stop));
            int idx = find_stop(stop);
            if (idx != -1) printf("%s\n", stops[idx].note);
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < nstops; i++) {
                printf("%s %s\n", stops[i].stop, stops[i].note);
            }
        }
    }
    for (int i = 0; i < nstops; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    return 0;
}