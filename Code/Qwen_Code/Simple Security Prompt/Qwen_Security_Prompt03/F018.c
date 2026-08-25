// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *word; int count; } Wf;
Wf *wf = NULL;
int n = 0;

int cmp(const void *x, const void *y) { return ((Wf*)y)->count - ((Wf*)x)->count; }

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *w = strtok(line, " \t");
                while (w) {
                    int idx = -1;
                    for (int i = 0; i < n; i++) if (strcmp(wf[i].word, w) == 0) { idx = i; break; }
                    if (idx != -1) wf[idx].count++;
                    else { wf = realloc(wf, (n + 1) * sizeof(Wf)); wf[n].word = strdup(w); wf[n].count = 1; n++; }
                    w = strtok(NULL, " \t");
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], w[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "WORD") == 0) {
            char *t = strchr(line, ' ');
            if (t) {
                t++;
                int idx = -1;
                for (int i = 0; i < n; i++) if (strcmp(wf[i].word, t) == 0) { idx = i; break; }
                if (idx != -1) wf[idx].count++;
                else { wf = realloc(wf, (n + 1) * sizeof(Wf)); wf[n].word = strdup(t); wf[n].count = 1; n++; }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *t = strchr(line, ' ');
            if (t) {
                t++;
                for (int i = 0; i < n; i++) if (strcmp(wf[i].word, t) == 0) {
                    free(wf[i].word);
                    for (int j = i; j < n - 1; j++) wf[j] = wf[j + 1];
                    n--; break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *t = strchr(line, ' ');
            if (t) {
                t++;
                for (int i = 0; i < n; i++) if (strcmp(wf[i].word, t) == 0) { printf("%d\n", wf[i].count); break; }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            qsort(wf, n, sizeof(Wf), cmp);
            for (int i = 0; i < n; i++) printf("%s %d\n", wf[i].word, wf[i].count);
        }
    }
    for (int i = 0; i < n; i++) free(wf[i].word);
    free(wf);
    return 0;
}