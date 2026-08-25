// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *email; char *phone; char *note; } Con;
Con *c = NULL;
int n = 0;

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256], b[256], d[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %s %s", a, b, d);
            c = realloc(c, (n + 1) * sizeof(Con));
            c[n].name = strdup(a); c[n].email = strdup(b); c[n].phone = strdup(d); c[n].note = strdup(""); n++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            sscanf(line, "%*s %s", a);
            char *note = strchr(line, ' '); note++; note = strchr(note, ' ');
            for (int i = 0; i < n; i++) if (strcmp(c[i].name, a) == 0) { free(c[i].note); c[i].note = strdup(note ? note + 1 : ""); break; }
        } else if (strcmp(cmd, "MERGE") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            int ia = -1, ib = -1;
            for (int i = 0; i < n; i++) {
                if (strcmp(c[i].name, a) == 0) ia = i;
                if (strcmp(c[i].name, b) == 0) ib = i;
            }
            if (ia != -1 && ib != -1) {
                char *nn = malloc(strlen(c[ia].note) + strlen(c[ib].note) + 2);
                sprintf(nn, "%s %s", c[ia].note, c[ib].note);
                free(c[ia].note); c[ia].note = nn;
                free(c[ib].name); free(c[ib].email); free(c[ib].phone); free(c[ib].note);
                for (int i = ib; i < n - 1; i++) c[i] = c[i + 1];
                n--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < n; i++) if (strcmp(c[i].name, a) == 0) {
                free(c[i].name); free(c[i].email); free(c[i].phone); free(c[i].note);
                for (int j = i; j < n - 1; j++) c[j] = c[j + 1];
                n--; break;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < n; i++) if (strcmp(c[i].name, a) == 0) { printf("%s %s %s %s\n", c[i].name, c[i].email, c[i].phone, c[i].note); break; }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < n; i++) printf("%s %s %s %s\n", c[i].name, c[i].email, c[i].phone, c[i].note);
        }
    }
    for (int i = 0; i < n; i++) { free(c[i].name); free(c[i].email); free(c[i].phone); free(c[i].note); }
    free(c);
    return 0;
}