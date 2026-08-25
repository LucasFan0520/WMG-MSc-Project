// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *email; char *phone; } Con;
Con *c = NULL;
int n = 0;

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *n1 = strtok(line, ",");
                char *e1 = strtok(NULL, ",");
                char *p1 = strtok(NULL, ",");
                if (n1 && e1 && p1) {
                    c = realloc(c, (n + 1) * sizeof(Con));
                    c[n].name = strdup(n1); c[n].email = strdup(e1); c[n].phone = strdup(p1); n++;
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256], b[256], d[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %[^,],%[^,],%s", a, b, d);
            c = realloc(c, (n + 1) * sizeof(Con));
            c[n].name = strdup(a); c[n].email = strdup(b); c[n].phone = strdup(d); n++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < n; i++) if (strcmp(c[i].name, a) == 0) {
                free(c[i].name); free(c[i].email); free(c[i].phone);
                for (int j = i; j < n - 1; j++) c[j] = c[j + 1];
                n--; break;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < n; i++) if (strcmp(c[i].name, a) == 0) { printf("%s,%s,%s\n", c[i].name, c[i].email, c[i].phone); break; }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < n; i++) printf("%s,%s,%s\n", c[i].name, c[i].email, c[i].phone);
        }
    }
    for (int i = 0; i < n; i++) { free(c[i].name); free(c[i].email); free(c[i].phone); }
    free(c);
    return 0;
}