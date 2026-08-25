// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *email; char *phone; } Con;
Con c[10000]; int nc = 0;

int find(char *name) {
    for (int i = 0; i < nc; i++) if (!strcmp(c[i].name, name)) return i;
    return -1;
}

void load(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *n = strtok(line, ",");
        char *e = strtok(NULL, ",");
        char *p = strtok(NULL, ",");
        if (n && e && p) {
            c[nc].name = strdup(n); c[nc].email = strdup(e); c[nc++].phone = strdup(p);
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load(argv[1]);
    char cmd[20], name[100], email[100], phone[100], line[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            fgets(line, sizeof(line), stdin);
            line[strcspn(line, "\n")] = 0;
            if (line[0] == ' ') memmove(line, line + 1, strlen(line));
            char *n = strtok(line, ",");
            char *e = strtok(NULL, ",");
            char *p = strtok(NULL, ",");
            if (n && e && p) {
                c[nc].name = strdup(n); c[nc].email = strdup(e); c[nc++].phone = strdup(p);
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name); int idx = find(name);
            if (idx != -1) {
                free(c[idx].name); free(c[idx].email); free(c[idx].phone);
                for (int i = idx; i < nc - 1; i++) c[i] = c[i + 1];
                nc--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", name); int idx = find(name);
            if (idx != -1) printf("%s,%s,%s\n", c[idx].name, c[idx].email, c[idx].phone);
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < nc; i++) printf("%s,%s,%s\n", c[i].name, c[i].email, c[i].phone);
        }
    }
    for (int i = 0; i < nc; i++) { free(c[i].name); free(c[i].email); free(c[i].phone); }
    return 0;
}