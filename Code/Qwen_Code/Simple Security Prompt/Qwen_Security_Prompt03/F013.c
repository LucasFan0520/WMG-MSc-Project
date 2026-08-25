// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *note; char *list; } Task;
Task *t = NULL;
int n = 0;

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], title[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "TODO") == 0) {
            sscanf(line, "%*s %s", title);
            char *note = strchr(line, ' '); note++; note = strchr(note, ' ');
            t = realloc(t, (n + 1) * sizeof(Task));
            t[n].title = strdup(title); t[n].note = strdup(note ? note + 1 : ""); t[n].list = strdup("todo"); n++;
        } else if (strcmp(cmd, "START") == 0) {
            sscanf(line, "%*s %s", title);
            for (int i = 0; i < n; i++) if (strcmp(t[i].title, title) == 0 && strcmp(t[i].list, "todo") == 0) { free(t[i].list); t[i].list = strdup("doing"); break; }
        } else if (strcmp(cmd, "DONE") == 0) {
            sscanf(line, "%*s %s", title);
            for (int i = 0; i < n; i++) if (strcmp(t[i].title, title) == 0 && strcmp(t[i].list, "doing") == 0) { free(t[i].list); t[i].list = strdup("done"); break; }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s", title);
            for (int i = 0; i < n; i++) {
                if (strcmp(t[i].title, title) == 0) {
                    free(t[i].title); free(t[i].note); free(t[i].list);
                    for (int j = i; j < n - 1; j++) t[j] = t[j + 1];
                    n--; break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", title);
            for (int i = 0; i < n; i++) if (strcmp(t[i].title, title) == 0) { printf("%s %s\n", t[i].list, t[i].note); break; }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("todo:"); for (int i = 0; i < n; i++) if (strcmp(t[i].list, "todo") == 0) printf(" %s", t[i].title); printf("\n");
            printf("doing:"); for (int i = 0; i < n; i++) if (strcmp(t[i].list, "doing") == 0) printf(" %s", t[i].title); printf("\n");
            printf("done:"); for (int i = 0; i < n; i++) if (strcmp(t[i].list, "done") == 0) printf(" %s", t[i].title); printf("\n");
        }
    }
    for (int i = 0; i < n; i++) { free(t[i].title); free(t[i].note); free(t[i].list); }
    free(t);
    return 0;
}