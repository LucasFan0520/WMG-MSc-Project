// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} Con;

Con cons[10000];
int cc = 0;

int find_name(char *name) {
    for (int i = 0; i < cc; i++) if (strcmp(cons[i].name, name) == 0) return i;
    return -1;
}

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *n = strtok(line, ",");
        char *e = strtok(NULL, ",");
        char *p = strtok(NULL, ",");
        if (n && e && p) {
            cons[cc].name = strdup(n);
            cons[cc].email = strdup(e);
            cons[cc].phone = strdup(p);
            cc++;
        }
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char *n = strtok(rest, ",");
            char *e = strtok(NULL, ",");
            char *p = strtok(NULL, ",");
            if (n && e && p) {
                cons[cc].name = strdup(n);
                cons[cc].email = strdup(e);
                cons[cc].phone = strdup(p);
                cc++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(cons[idx].name); free(cons[idx].email); free(cons[idx].phone);
                for (int i = idx; i < cc - 1; i++) cons[i] = cons[i+1];
                cc--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_name(name);
            if (idx != -1) printf("%s,%s,%s\n", cons[idx].name, cons[idx].email, cons[idx].phone);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < cc; i++) printf("%s,%s,%s\n", cons[i].name, cons[i].email, cons[i].phone);
        }
    }
    for (int i = 0; i < cc; i++) {
        free(cons[i].name); free(cons[i].email); free(cons[i].phone);
    }
    return 0;
}