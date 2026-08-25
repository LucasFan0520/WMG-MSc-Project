// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} Record;

Record *recs = NULL;
int rc = 0;

int find_rec(const char *name) {
    for (int i = 0; i < rc; i++) {
        if (strcmp(recs[i].name, name) == 0) return i;
    }
    return -1;
}

int main(void) {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char rest[4096];
        a1[0] = '\0';
        rest[0] = '\0';
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, a1, rest);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            char *colon = strchr(rest, ':');
            if (!colon) continue;
            *colon = '\0';
            int len = atoi(rest);
            if (len < 0) continue;
            char *data_start = colon + 1;
            if ((int)strlen(data_start) < len) continue;
            char *data = malloc(len + 1);
            if (!data) return 1;
            memcpy(data, data_start, len);
            data[len] = '\0';
            recs = realloc(recs, (rc + 1) * sizeof(Record));
            if (!recs) {
                free(data);
                return 1;
            }
            recs[rc].name = strdup(a1);
            recs[rc].data = data;
            rc++;
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            int ri = find_rec(a1);
            if (ri >= 0) {
                free(recs[ri].name);
                free(recs[ri].data);
                for (int k = ri; k < rc - 1; k++) {
                    recs[k] = recs[k + 1];
                }
                rc--;
            }
        } else if (strcmp(cmd, "SHOW") == 0 && n >= 2) {
            int ri = find_rec(a1);
            if (ri >= 0) {
                for (char *p = recs[ri].data; *p; p++) {
                    putchar(*p == ' ' ? '_' : *p);
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%s\n", recs[i].name);
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        free(recs[i].name);
        free(recs[i].data);
    }
    free(recs);
    return 0;
}