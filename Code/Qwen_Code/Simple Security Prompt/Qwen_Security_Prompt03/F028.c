// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *stop; char *note; } Stop;
Stop *stops = NULL;
int ns = 0;

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        int idx;
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            stops = realloc(stops, (ns + 1) * sizeof(Stop));
            stops[ns].stop = strdup(a);
            stops[ns].note = strdup(t);
            ns++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            sscanf(line, "%*s %d %s", &idx, a);
            if (idx < 0 || idx > ns) continue;
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++; t = strchr(t, ' '); t++;
            stops = realloc(stops, (ns + 1) * sizeof(Stop));
            for (int i = ns; i > idx; i--) stops[i] = stops[i - 1];
            stops[idx].stop = strdup(a);
            stops[idx].note = strdup(t);
            ns++;
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from, to;
            sscanf(line, "%*s %d %d", &from, &to);
            if (from < 0 || from >= ns || to < 0 || to >= ns) continue;
            Stop tmp = stops[from];
            if (from < to) {
                for (int i = from; i < to; i++) stops[i] = stops[i + 1];
            } else {
                for (int i = from; i > to; i--) stops[i] = stops[i - 1];
            }
            stops[to] = tmp;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %d", &idx);
            if (idx < 0 || idx >= ns) continue;
            free(stops[idx].stop);
            free(stops[idx].note);
            for (int i = idx; i < ns - 1; i++) stops[i] = stops[i + 1];
            ns--;
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < ns; i++) {
                if (strcmp(stops[i].stop, a) == 0) {
                    printf("%s\n", stops[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < ns; i++) {
                printf("%s %s\n", stops[i].stop, stops[i].note);
            }
        }
    }
    for (int i = 0; i < ns; i++) {
        free(stops[i].stop);
        free(stops[i].note);
    }
    free(stops);
    return 0;
}