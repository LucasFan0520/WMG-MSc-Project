// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *data; } Rec;
Rec recs[10000]; int nrecs = 0;

int find_rec(char *name) {
    for (int i = 0; i < nrecs; i++) if (!strcmp(recs[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], raw[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", name);
            fgets(raw, sizeof(raw), stdin);
            raw[strcspn(raw, "\n")] = 0;
            if (raw[0] == ' ') memmove(raw, raw + 1, strlen(raw));
            char *colon = strchr(raw, ':');
            if (colon) {
                *colon = 0;
                int len = atoi(raw);
                char *data = colon + 1;
                if ((int)strlen(data) == len) {
                    recs[nrecs].name = strdup(name);
                    recs[nrecs++].data = strdup(data);
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) {
                free(recs[idx].name); free(recs[idx].data);
                for (int i = idx; i < nrecs - 1; i++) recs[i] = recs[i + 1];
                nrecs--;
            }
        } else if (!strcmp(cmd, "SHOW")) {
            scanf("%s", name);
            int idx = find_rec(name);
            if (idx != -1) {
                for (char *c = recs[idx].data; *c; c++) printf("%c", *c == ' ' ? '_' : *c);
                printf("\n");
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < nrecs; i++) {
                printf("%s\n", recs[i].name);
            }
        }
    }
    for (int i = 0; i < nrecs; i++) { free(recs[i].name); free(recs[i].data); }
    return 0;
}