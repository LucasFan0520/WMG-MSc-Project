// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *tag; char *msg; } Rec;
Rec *recs = NULL; int rc = 0;

int find_tag(const char *t) {
    for (int i = 0; i < rc; i++) if (strcmp(recs[i].tag, t) == 0) return i;
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
        
        if (strcmp(c1, "ADD") == 0) {
            int i = find_tag(c2);
            if (i >= 0) { free(recs[i].msg); recs[i].msg = strdup(c3); }
            else { recs = realloc(recs, (rc+1)*sizeof(Rec)); recs[rc].tag = strdup(c2); recs[rc].msg = strdup(c3); rc++; }
        } else if (strcmp(c1, "CONCAT") == 0) {
            int i1 = find_tag(c2), i2 = find_tag(c3);
            if (i1 >= 0 && i2 >= 0) {
                char *nm = malloc(strlen(recs[i1].msg) + strlen(recs[i2].msg) + 2);
                sprintf(nm, "%s %s", recs[i1].msg, recs[i2].msg);
                int i3 = find_tag(c4);
                if (i3 >= 0) { free(recs[i3].msg); recs[i3].msg = nm; }
                else { recs = realloc(recs, (rc+1)*sizeof(Rec)); recs[rc].tag = strdup(c4); recs[rc].msg = nm; rc++; }
            }
        } else if (strcmp(c1, "REMOVE") == 0) {
            int i = find_tag(c2);
            if (i >= 0) { free(recs[i].tag); free(recs[i].msg); for(int j=i; j<rc-1; j++) recs[j]=recs[j+1]; rc--; }
        } else if (strcmp(c1, "FIND") == 0) {
            int i = find_tag(c2);
            if (i >= 0) { for(char *p=recs[i].msg; *p; p++) putchar(*p==' '?'_':*p); putchar('\n'); }
        } else if (strcmp(c1, "REPORT") == 0) {
            for (int i = 0; i < rc; i++) printf("%s %s\n", recs[i].tag, recs[i].msg);
        }
    }
    for (int i = 0; i < rc; i++) { free(recs[i].tag); free(recs[i].msg); }
    free(recs);
    return 0;
}