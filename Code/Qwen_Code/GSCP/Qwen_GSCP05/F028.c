// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

Stop *stops = NULL;
int sc = 0;

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[256];
        char rest[2048];
        a1[0] = '\0';
        a2[0] = '\0';
        rest[0] = '\0';
        int n = sscanf(line, "%31s", cmd);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            n = sscanf(line, "%31s %255s %2047[^\n]", cmd, a1, rest);
            if (n >= 3) {
                stops = realloc(stops, (sc + 1) * sizeof(Stop));
                if (!stops) return 1;
                stops[sc].stop = strdup(a1);
                stops[sc].note = strdup(rest);
                sc++;
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            n = sscanf(line, "%31s %d %255s %2047[^\n]", cmd, &idx, a1, rest);
            if (n >= 4) {
                if (idx < 0 || idx > sc) continue;
                stops = realloc(stops, (sc + 1) * sizeof(Stop));
                if (!stops) return 1;
                for (int i = sc; i > idx; i--) {
                    stops[i] = stops[i - 1];
                }
                stops[idx].stop = strdup(a1);
                stops[idx].note = strdup(rest);
                sc++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            int fi, ti;
            n = sscanf(line, "%31s %d %d", cmd, &fi, &ti);
            if (n >= 3) {
                if (fi < 0 || fi >= sc || ti < 0 || ti >= sc) continue;
                Stop tmp = stops[fi];
                if (fi < ti) {
                    for (int i = fi; i < ti; i++) {
                        stops[i] = stops[i + 1];
                    }
                } else {
                    for (int i = fi; i > ti; i--) {
                        stops[i] = stops[i - 1];
                    }
                }
                stops[ti] = tmp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx;
            n = sscanf(line, "%31s %d", cmd, &idx);
            if (n >= 2) {
                if (idx < 0 || idx >= sc) continue;
                free(stops[idx].stop);
                free(stops[idx].note);
                for (int i = idx; i < sc - 1; i++) {
                    stops[i] = stops[i + 1];
                }
                sc--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            n = sscanf(line, "%31s %255s", cmd, a1);
            if (n >= 2) {
                for (int i = 0; i < sc; i++) {
                    if (strcmp(stops[i].stop, a1) == 0) {
                        printf("%s\n", stops[i].note);
                        break;
                    }
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
    free(stops);
    return 0;
}