// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Stud;
typedef struct { char *name; Stud *studs; int n; } Cls;
Cls *cls = NULL;
int nc = 0;

int find_cls(const char *name) {
    for (int i = 0; i < nc; i++) if (strcmp(cls[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256], b[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "CLASS") == 0) {
            sscanf(line, "%*s %s", a);
            if (find_cls(a) == -1) {
                cls = realloc(cls, (nc + 1) * sizeof(Cls));
                cls[nc].name = strdup(a);
                cls[nc].studs = NULL;
                cls[nc].n = 0;
                nc++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++; t = strchr(t, ' '); t++;
            int idx = find_cls(a);
            if (idx != -1) {
                cls[idx].studs = realloc(cls[idx].studs, (cls[idx].n + 1) * sizeof(Stud));
                cls[idx].studs[cls[idx].n].name = strdup(b);
                cls[idx].studs[cls[idx].n].note = strdup(t);
                cls[idx].n++;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            sscanf(line, "%*s %s %s %s", a, b, cmd);
            int from = find_cls(a), to = find_cls(b);
            if (from != -1 && to != -1) {
                for (int i = 0; i < cls[from].n; i++) {
                    if (strcmp(cls[from].studs[i].name, cmd) == 0) {
                        cls[to].studs = realloc(cls[to].studs, (cls[to].n + 1) * sizeof(Stud));
                        cls[to].studs[cls[to].n] = cls[from].studs[i];
                        cls[to].n++;
                        for (int j = i; j < cls[from].n - 1; j++) cls[from].studs[j] = cls[from].studs[j + 1];
                        cls[from].n--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            int idx = find_cls(a);
            if (idx != -1) {
                for (int i = 0; i < cls[idx].n; i++) {
                    if (strcmp(cls[idx].studs[i].name, b) == 0) {
                        free(cls[idx].studs[i].name);
                        free(cls[idx].studs[i].note);
                        for (int j = i; j < cls[idx].n - 1; j++) cls[idx].studs[j] = cls[idx].studs[j + 1];
                        cls[idx].n--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            sscanf(line, "%*s %s", a);
            int idx = find_cls(a);
            if (idx != -1) {
                for (int i = 0; i < cls[idx].n; i++) {
                    free(cls[idx].studs[i].name);
                    free(cls[idx].studs[i].note);
                }
                free(cls[idx].studs);
                free(cls[idx].name);
                for (int i = idx; i < nc - 1; i++) cls[i] = cls[i + 1];
                nc--;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < nc; i++) {
                printf("%s:\n", cls[i].name);
                for (int j = 0; j < cls[i].n; j++) {
                    printf("  %s %s\n", cls[i].studs[j].name, cls[i].studs[j].note);
                }
            }
        }
    }
    for (int i = 0; i < nc; i++) {
        for (int j = 0; j < cls[i].n; j++) {
            free(cls[i].studs[j].name);
            free(cls[i].studs[j].note);
        }
        free(cls[i].studs);
        free(cls[i].name);
    }
    free(cls);
    return 0;
}