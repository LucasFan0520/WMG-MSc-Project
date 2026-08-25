// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REC 10000
#define MAX_LEN 1000

typedef struct {
    char name[100];
    char data[MAX_LEN];
} Rec;

Rec recs[MAX_REC];
int rcount = 0;

int find_rec(const char *name) {
    for (int i = 0; i < rcount; i++) if (strcmp(recs[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], raw[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s", name);
            scanf("%s", raw);
            char *colon = strchr(raw, ':');
            if (colon) {
                int len = atoi(raw);
                if (len >= 0 && len == (int)strlen(colon + 1)) {
                    if (rcount < MAX_REC) {
                        strcpy(recs[rcount].name, name);
                        strcpy(recs[rcount].data, colon + 1);
                        rcount++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) {
                for (int i = idx; i < rcount - 1; i++) recs[i] = recs[i + 1];
                rcount--;
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) {
                for (char *p = recs[idx].data; *p; p++) {
                    putchar(*p == ' ' ? '_' : *p);
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s\n", recs[i].name);
            }
        }
    }
    return 0;
}