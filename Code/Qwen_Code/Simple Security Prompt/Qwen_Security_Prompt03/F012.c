// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Per;
Per *q = NULL;
int n = 0;

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], name[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            sscanf(line, "%*s %s", name);
            char *note = strchr(line, ' '); note++; note = strchr(note, ' ');
            q = realloc(q, (n + 1) * sizeof(Per));
            q[n].name = strdup(name); q[n].note = strdup(note ? note + 1 : ""); n++;
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (n > 0) {
                printf("%s\n", q[0].name);
                free(q[0].name); free(q[0].note);
                for (int i = 0; i < n - 1; i++) q[i] = q[i + 1];
                n--;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            sscanf(line, "%*s %s", name);
            for (int i = 0; i < n; i++) {
                if (strcmp(q[i].name, name) == 0) {
                    free(q[i].name); free(q[i].note);
                    for (int j = i; j < n - 1; j++) q[j] = q[j + 1];
                    n--; break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", name);
            for (int i = 0; i < n; i++) if (strcmp(q[i].name, name) == 0) { printf("%s\n", q[i].note); break; }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = 0; i < n; i++) printf("%s %s\n", q[i].name, q[i].note);
        }
    }
    for (int i = 0; i < n; i++) { free(q[i].name); free(q[i].note); }
    free(q);
    return 0;
}