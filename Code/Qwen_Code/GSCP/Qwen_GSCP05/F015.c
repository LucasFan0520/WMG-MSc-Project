// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *email; char *phone; char *note; } Con;
Con *cons = NULL; int cc = 0;

int find_con(const char *n) {
    for (int i = 0; i < cc; i++) if (strcmp(cons[i].name, n) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c3[256], c4[256], c5[256];
        c2[0] = c3[0] = c4[0] = c5[0] = 0;
        sscanf(line, "%19s %255s %255s %255s %255[^\n]", c1, c2, c3, c4, c5);
        
        if (strcmp(c1, "ADD") == 0) {
            if (find_con(c2) < 0) {
                cons = realloc(cons, (cc+1)*sizeof(Con));
                cons[cc].name = strdup(c2); cons[cc].email = strdup(c3); cons[cc].phone = strdup(c4); cons[cc].note = strdup(""); cc++;
            }
        } else if (strcmp(c1, "NOTE") == 0) {
            int i = find_con(c2);
            if (i >= 0) { free(cons[i].note); cons[i].note = strdup(c3); }
        } else if (strcmp(c1, "MERGE") == 0) {
            int iK = find_con(c2), iR = find_con(c3);
            if (iK >= 0 && iR >= 0) {
                char *nn = malloc(strlen(cons[iK].note) + strlen(cons[iR].note) + 2);
                sprintf(nn, "%s %s", cons[iK].note, cons[iR].note);
                free(cons[iK].note); cons[iK].note = nn;
                free(cons[iR].name); free(cons[iR].email); free(cons[iR].phone); free(cons[iR].note);
                for (int j = iR; j < cc-1; j++) cons[j] = cons[j+1];
                cc--;
            }
        } else if (strcmp(c1, "DELETE") == 0) {
            int i = find_con(c2);
            if (i >= 0) {
                free(cons[i].name); free(cons[i].email); free(cons[i].phone); free(cons[i].note);
                for (int j = i; j < cc-1; j++) cons[j] = cons[j+1];
                cc--;
            }
        } else if (strcmp(c1, "FIND") == 0) {
            int i = find_con(c2);
            if (i >= 0) printf("%s %s %s %s\n", cons[i].name, cons[i].email, cons[i].phone, cons[i].note);
        } else if (strcmp(c1, "LIST") == 0) {
            for (int i = 0; i < cc; i++) printf("%s %s %s %s\n", cons[i].name, cons[i].email, cons[i].phone, cons[i].note);
        }
    }
    for (int i = 0; i < cc; i++) { free(cons[i].name); free(cons[i].email); free(cons[i].phone); free(cons[i].note); }
    free(cons);
    return 0;
}