// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Rec;
Rec recs[100000]; int nrecs = 0;

int find_rec(char *name) {
    for (int i = 0; i < nrecs; i++) if (!strcmp(recs[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], note[1000]; int n;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "BATCH")) {
            scanf("%d", &n);
            Rec *batch = malloc(sizeof(Rec) * n);
            int valid = 1;
            for (int i = 0; i < n; i++) {
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = 0;
                if (name[0] == ' ') memmove(name, name + 1, strlen(name));
                char *space = strchr(name, ' ');
                if (!space) { valid = 0; continue; }
                *space = 0;
                char *nt = space + 1;
                while(*nt == ' ') nt++;
                batch[i].name = strdup(name);
                batch[i].note = strdup(nt);
            }
            if (valid) {
                for (int i = 0; i < n; i++) {
                    recs[nrecs++] = batch[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    if (batch[i].name) free(batch[i].name);
                    if (batch[i].note) free(batch[i].note);
                }
            }
            free(batch);
        } else if (!strcmp(cmd, "DELETE")) {
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            if (name[0] == ' ') memmove(name, name + 1, strlen(name));
            int idx = find_rec(name);
            if (idx != -1) {
                free(recs[idx].name);
                free(recs[idx].note);
                for (int i = idx; i < nrecs - 1; i++) recs[i] = recs[i + 1];
                nrecs--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            if (name[0] == ' ') memmove(name, name + 1, strlen(name));
            int idx = find_rec(name);
            if (idx != -1) printf("%s %s\n", recs[idx].name, recs[idx].note);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < nrecs; i++) {
                printf("%s %s\n", recs[i].name, recs[i].note);
            }
        }
    }
    for (int i = 0; i < nrecs; i++) {
        free(recs[i].name);
        free(recs[i].note);
    }
    return 0;
}