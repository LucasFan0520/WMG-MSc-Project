/* F028.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *stop;
    char *note;
} Stop;

static Stop *stops = NULL;
static int scount = 0;
static int scap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_stop(const char *name) {
    for (int i = 0; i < scount; i++)
        if (strcmp(stops[i].stop, name) == 0) return i;
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char stop[256] = {0};
            int off2 = 0;
            if (sscanf(line + offset, "%255s%n", stop, &off2) != 1) continue;
            const char *note = line + offset + off2;
            while (*note == ' ') note++;
            if (scount >= scap) {
                scap = scap == 0 ? 8 : scap * 2;
                stops = realloc(stops, sizeof(Stop) * scap);
            }
            stops[scount].stop = dup_str(stop);
            stops[scount].note = dup_str(note);
            scount++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            int idx = 0;
            char stop[256] = {0};
            int off2 = 0, off3 = 0;
            if (sscanf(line + offset, "%d%n %255s%n", &idx, &off2, stop, &off3) != 2) continue;
            if (idx < 0 || idx > scount) continue;
            const char *note = line + offset + off2 + off3;
            while (*note == ' ') note++;
            if (scount >= scap) {
                scap = scap == 0 ? 8 : scap * 2;
                stops = realloc(stops, sizeof(Stop) * scap);
            }
            for (int i = scount; i > idx; i--) stops[i] = stops[i - 1];
            stops[idx].stop = dup_str(stop);
            stops[idx].note = dup_str(note);
            scount++;
        } else if (strcmp(cmd, "MOVE") == 0) {
            int from = 0, to = 0;
            if (sscanf(line + offset, "%d %d", &from, &to) != 2) continue;
            if (from < 0 || from >= scount || to < 0 || to >= scount) continue;
            Stop s = stops[from];
            if (from < to) {
                for (int i = from; i < to; i++) stops[i] = stops[i + 1];
            } else {
                for (int i = from; i > to; i--) stops[i] = stops[i - 1];
            }
            stops[to] = s;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            int idx = 0;
            if (sscanf(line + offset, "%d", &idx) != 1) continue;
            if (idx < 0 || idx >= scount) continue;
            free(stops[idx].stop);
            free(stops[idx].note);
            for (int i = idx; i < scount - 1; i++) stops[i] = stops[i + 1];
            scount--;
        } else if (strcmp(cmd, "FIND") == 0) {
            char stop[256] = {0};
            if (sscanf(line + offset, "%255s", stop) != 1) continue;
            int idx = find_stop(stop);
            if (idx >= 0) printf("%s\n", stops[idx].note);
            else printf("NOT_FOUND\n");
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