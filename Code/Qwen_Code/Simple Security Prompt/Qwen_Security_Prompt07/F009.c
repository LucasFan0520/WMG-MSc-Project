// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *desc; } Event;
typedef struct { char *name; Event *evts; int n, cap; } Group;
Group *grps = 0; int ng = 0, cg = 0;

int find_grp(char *name) {
    for (int i=0; i<ng; i++) if (!strcmp(grps[i].name, name)) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256], c[256], desc[1700];
        b[0] = c[0] = desc[0] = 0;
        sscanf(line, "%s %s %s %s %[^\n]", cmd, a, b, c, desc);
        if (!strcmp(cmd, "EVENT")) {
            char *d = strstr(line, a) + strlen(a) + 1;
            int i = find_grp(a);
            if (i < 0) {
                if (ng == cg) { cg = cg ? cg*2 : 4; grps = realloc(grps, cg*sizeof(Group)); }
                i = ng++; grps[i].name = strdup(a); grps[i].evts = 0; grps[i].n = 0; grps[i].cap = 0;
            }
            if (grps[i].n == grps[i].cap) {
                grps[i].cap = grps[i].cap ? grps[i].cap*2 : 4;
                grps[i].evts = realloc(grps[i].evts, grps[i].cap*sizeof(Event));
            }
            grps[i].evts[grps[i].n++].desc = strdup(d);
        } else if (!strcmp(cmd, "MERGE")) {
            int iA = find_grp(a), iB = find_grp(b);
            if (find_grp(c) < 0) {
                if (ng == cg) { cg = cg ? cg*2 : 4; grps = realloc(grps, cg*sizeof(Group)); }
                int iC = ng++; grps[iC].name = strdup(c); grps[iC].evts = 0; grps[iC].n = 0; grps[iC].cap = 0;
                int tot = (iA>=0?grps[iA].n:0) + (iB>=0?grps[iB].n:0);
                grps[iC].cap = tot ? tot : 4; grps[iC].evts = malloc(grps[iC].cap*sizeof(Event));
                if (iA >= 0) for (int j=0; j<grps[iA].n; j++) grps[iC].evts[grps[iC].n++].desc = strdup(grps[iA].evts[j].desc);
                if (iB >= 0) for (int j=0; j<grps[iB].n; j++) grps[iC].evts[grps[iC].n++].desc = strdup(grps[iB].evts[j].desc);
            }
        } else if (!strcmp(cmd, "DELETEGROUP")) {
            int i = find_grp(a);
            if (i >= 0) {
                for (int j=0; j<grps[i].n; j++) free(grps[i].evts[j].desc);
                free(grps[i].evts); free(grps[i].name);
                for (int j=i; j<ng-1; j++) grps[j] = grps[j+1]; ng--;
            }
        } else if (!strcmp(cmd, "COUNT")) {
            int i = find_grp(a); printf("%d\n", i >= 0 ? grps[i].n : 0);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i=0; i<ng; i++) printf("%s %d\n", grps[i].name, grps[i].n);
        }
    }
    return 0;
}