// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *stop, *note; } Stop;
Stop *stops = 0; int n = 0, cap = 0;

int find(char *stop) {
    for (int i = 0; i < n; i++) if (!strcmp(stops[i].stop, stop)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], rest[2048]; int idx, idx2;
        a[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "ADD")) sscanf(line, "%*s %s %[^\n]", a, rest);
        else if (!strcmp(cmd, "INSERT")) sscanf(line, "%*s %d %s %[^\n]", &idx, a, rest);
        else if (!strcmp(cmd, "MOVE")) sscanf(line, "%*s %d %d", &idx, &idx2);
        else if (!strcmp(cmd, "REMOVE")) sscanf(line, "%*s %d", &idx);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "ADD")) {
            if (n == cap) { cap = cap ? cap * 2 : 4; stops = realloc(stops, cap * sizeof(Stop)); }
            stops[n].stop = strdup(a); stops[n].note = strdup(rest); n++;
        } else if (!strcmp(cmd, "INSERT")) {
            if (idx >= 0 && idx <= n) {
                if (n == cap) { cap = cap ? cap * 2 : 4; stops = realloc(stops, cap * sizeof(Stop)); }
                for (int i = n; i > idx; i--) stops[i] = stops[i - 1];
                stops[idx].stop = strdup(a); stops[idx].note = strdup(rest); n++;
            }
        } else if (!strcmp(cmd, "MOVE")) {
            if (idx >= 0 && idx < n && idx2 >= 0 && idx2 < n && idx != idx2) {
                Stop tmp = stops[idx];
                if (idx < idx2) {
                    for (int i = idx; i < idx2; i++) stops[i] = stops[i + 1];
                } else {
                    for (int i = idx; i > idx2; i--) stops[i] = stops[i - 1];
                }
                stops[idx2] = tmp;
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            if (idx >= 0 && idx < n) {
                free(stops[idx].stop); free(stops[idx].note);
                for (int i = idx; i < n - 1; i++) stops[i] = stops[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            int i = find(a);
            if (i >= 0) puts(stops[i].note);
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < n; i++) printf("%s %s\n", stops[i].stop, stops[i].note);
        }
    }
    return 0;
}