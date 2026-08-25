// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *word; int count; } WF;
WF *wfs = NULL; int wc = 0;

int find_wf(const char *w) {
    for (int i = 0; i < wc; i++) if (strcmp(wfs[i].word, w) == 0) return i;
    return -1;
}

int cmp_wf(const void *a, const void *b) {
    return ((WF*)b)->count - ((WF*)a)->count;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                int i = find_wf(line);
                if (i >= 0) wfs[i].count++;
                else { wfs = realloc(wfs, (wc+1)*sizeof(WF)); wfs[wc].word = strdup(line); wfs[wc].count = 1; wc++; }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char c1[20], c2[700];
        c2[0] = 0;
        sscanf(line, "%19s %700[^\n]", c1, c2);
        
        if (strcmp(c1, "WORD") == 0) {
            int i = find_wf(c2);
            if (i >= 0) wfs[i].count++;
            else { wfs = realloc(wfs, (wc+1)*sizeof(WF)); wfs[wc].word = strdup(c2); wfs[wc].count = 1; wc++; }
        } else if (strcmp(c1, "REMOVE") == 0) {
            int i = find_wf(c2);
            if (i >= 0) { free(wfs[i].word); for(int j=i; j<wc-1; j++) wfs[j]=wfs[j+1]; wc--; }
        } else if (strcmp(c1, "COUNT") == 0) {
            int i = find_wf(c2);
            printf("%d\n", i >= 0 ? wfs[i].count : 0);
        } else if (strcmp(c1, "TOP") == 0) {
            qsort(wfs, wc, sizeof(WF), cmp_wf);
            for (int i = 0; i < wc; i++) printf("%s %d\n", wfs[i].word, wfs[i].count);
        }
    }
    for (int i = 0; i < wc; i++) free(wfs[i].word);
    free(wfs);
    return 0;
}