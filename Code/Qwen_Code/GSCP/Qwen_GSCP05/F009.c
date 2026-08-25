// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *desc; } Ev;
typedef struct { char *name; Ev *evs; int ec; } Grp;
Grp *grps = NULL; int gc = 0;

int find_grp(const char *n) {
    for (int i = 0; i < gc; i++) if (strcmp(grps[i].name, n) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c3[256], c4[256];
        c2[0] = c3[0] = c4[0] = 0;
        sscanf(line, "%19s %255s %255s %255[^\n]", c1, c2, c3, c4);
        
        if (strcmp(c1, "EVENT") == 0) {
            int i = find_grp(c2);
            if (i < 0) { grps = realloc(grps, (gc+1)*sizeof(Grp)); grps[gc].name = strdup(c2); grps[gc].evs = NULL; grps[gc].ec = 0; i = gc++; }
            grps[i].evs = realloc(grps[i].evs, (grps[i].ec+1)*sizeof(Ev));
            grps[i].evs[grps[i].ec++].desc = strdup(c3);
        } else if (strcmp(c1, "MERGE") == 0) {
            int iA = find_grp(c2), iB = find_grp(c3);
            if (iA >= 0 && iB >= 0) {
                int iC = find_grp(c4);
                if (iC < 0) { grps = realloc(grps, (gc+1)*sizeof(Grp)); grps[gc].name = strdup(c4); grps[gc].evs = NULL; grps[gc].ec = 0; iC = gc++; }
                for (int j = 0; j < grps[iA].ec; j++) {
                    grps[iC].evs = realloc(grps[iC].evs, (grps[iC].ec+1)*sizeof(Ev));
                    grps[iC].evs[grps[iC].ec++].desc = strdup(grps[iA].evs[j].desc);
                }
                for (int j = 0; j < grps[iB].ec; j++) {
                    grps[iC].evs = realloc(grps[iC].evs, (grps[iC].ec+1)*sizeof(Ev));
                    grps[iC].evs[grps[iC].ec++].desc = strdup(grps[iB].evs[j].desc);
                }
            }
        } else if (strcmp(c1, "DELETEGROUP") == 0) {
            int i = find_grp(c2);
            if (i >= 0) {
                free(grps[i].name);
                for (int j = 0; j < grps[i].ec; j++) free(grps[i].evs[j].desc);
                free(grps[i].evs);
                for (int j = i; j < gc-1; j++) grps[j] = grps[j+1];
                gc--;
            }
        } else if (strcmp(c1, "COUNT") == 0) {
            int i = find_grp(c2);
            printf("%d\n", i >= 0 ? grps[i].ec : 0);
        } else if (strcmp(c1, "REPORT") == 0) {
            for (int i = 0; i < gc; i++) printf("%s %d\n", grps[i].name, grps[i].ec);
        }
    }
    for (int i = 0; i < gc; i++) {
        free(grps[i].name);
        for (int j = 0; j < grps[i].ec; j++) free(grps[i].evs[j].desc);
        free(grps[i].evs);
    }
    free(grps);
    return 0;
}