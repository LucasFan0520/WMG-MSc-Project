// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *data;
} Record;

Record *recs = NULL;
int rcount = 0;

int find_rec(const char *name) {
    for (int i = 0; i < rcount; i++) {
        if (strcmp(recs[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char name[100], raw[2000];
            scanf("%s %s", name, raw);
            char *colon = strchr(raw, ':');
            if (colon) {
                int len = atoi(raw);
                char *data = colon + 1;
                if ((int)strlen(data) == len) {
                    int idx = find_rec(name);
                    if (idx != -1) {
                        free(recs[idx].data);
                        recs[idx].data = strdup(data);
                    } else {
                        recs = realloc(recs, sizeof(Record) * (rcount + 1));
                        recs[rcount].name = strdup(name);
                        recs[rcount].data = strdup(data);
                        rcount++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) {
                free(recs[idx].name);
                free(recs[idx].data);
                for (int i = idx; i < rcount - 1; i++) recs[i] = recs[i+1];
                rcount--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) {
                for (char *p = recs[idx].data; *p; p++) {
                    if (*p == ' ') putchar('_');
                    else putchar(*p);
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s\n", recs[i].name);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(recs[i].name);
        free(recs[i].data);
    }
    free(recs);
    return 0;
}