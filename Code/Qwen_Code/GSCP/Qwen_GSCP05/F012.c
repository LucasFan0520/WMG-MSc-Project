// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Per;
Per *q = NULL; int qc = 0;

int find_per(const char *n) {
    for (int i = 0; i < qc; i++) if (strcmp(q[i].name, n) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c3[700];
        c2[0] = c3[0] = 0;
        sscanf(line, "%19s %255s %700[^\n]", c1, c2, c3);
        
        if (strcmp(c1, "JOIN") == 0 || strcmp(c1, "REJOIN") == 0) {
            q = realloc(q, (qc+1)*sizeof(Per));
            q[qc].name = strdup(c2); q[qc].note = strdup(c3); qc++;
        } else if (strcmp(c1, "SERVE") == 0) {
            if (qc > 0) {
                printf("%s\n", q[0].name);
                free(q[0].name); free(q[0].note);
                for (int i = 0; i < qc-1; i++) q[i] = q[i+1];
                qc--;
            }
        } else if (strcmp(c1, "CANCEL") == 0) {
            int i = find_per(c2);
            if (i >= 0) { free(q[i].name); free(q[i].note); for(int j=i; j<qc-1; j++) q[j]=q[j+1]; qc--; }
        } else if (strcmp(c1, "FIND") == 0) {
            int i = find_per(c2);
            if (i >= 0) printf("%s\n", q[i].note);
        } else if (strcmp(c1, "QUEUE") == 0) {
            for (int i = 0; i < qc; i++) printf("%s\n", q[i].name);
        }
    }
    for (int i = 0; i < qc; i++) { free(q[i].name); free(q[i].note); }
    free(q);
    return 0;
}