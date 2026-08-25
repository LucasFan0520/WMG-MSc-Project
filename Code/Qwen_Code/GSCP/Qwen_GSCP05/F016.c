// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *email; char *phone; } Con;
Con *cons = NULL; int cc = 0;

int find_con(const char *n) {
    for (int i = 0; i < cc; i++) if (strcmp(cons[i].name, n) == 0) return i;
    return -1;
}

void add_con(const char *n, const char *e, const char *p) {
    if (find_con(n) < 0) {
        cons = realloc(cons, (cc+1)*sizeof(Con));
        cons[cc].name = strdup(n); cons[cc].email = strdup(e); cons[cc].phone = strdup(p); cc++;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *n = strtok(line, ","), *e = strtok(NULL, ","), *p = strtok(NULL, ",");
                if (n && e && p) add_con(n, e, p);
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[256], c3[256], c4[256];
        c2[0] = c3[0] = c4[0] = 0;
        sscanf(line, "%19s %255[^,],%255[^,],%255[^\n]", c1, c2, c3, c4);
        
        if (strcmp(c1, "ADD") == 0) add_con(c2, c3, c4);
        else if (strcmp(c1, "DELETE") == 0) {
            int i = find_con(c2);
            if (i >= 0) { free(cons[i].name); free(cons[i].email); free(cons[i].phone); for(int j=i; j<cc-1; j++) cons[j]=cons[j+1]; cc--; }
        } else if (strcmp(c1, "FIND") == 0) {
            int i = find_con(c2);
            if (i >= 0) printf("%s %s %s\n", cons[i].name, cons[i].email, cons[i].phone);
        } else if (strcmp(c1, "REPORT") == 0) {
            for (int i = 0; i < cc; i++) printf("%s %s %s\n", cons[i].name, cons[i].email, cons[i].phone);
        }
    }
    for (int i = 0; i < cc; i++) { free(cons[i].name); free(cons[i].email); free(cons[i].phone); }
    free(cons);
    return 0;
}