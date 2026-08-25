// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *word; int count; } WF;
WF wf[10000]; int nw = 0;

int find(char *word) {
    for (int i = 0; i < nw; i++) if (!strcmp(wf[i].word, word)) return i;
    return -1;
}

void load(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) > 0) {
            int idx = find(line);
            if (idx != -1) wf[idx].count++;
            else { wf[nw].word = strdup(line); wf[nw++].count = 1; }
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load(argv[1]);
    char cmd[20], text[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "WORD") || !strcmp(cmd, "REMOVE") || !strcmp(cmd, "COUNT")) {
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            if (text[0] == ' ') memmove(text, text + 1, strlen(text));
            if (!strcmp(cmd, "WORD")) {
                int idx = find(text);
                if (idx != -1) wf[idx].count++;
                else { wf[nw].word = strdup(text); wf[nw++].count = 1; }
            } else if (!strcmp(cmd, "REMOVE")) {
                int idx = find(text);
                if (idx != -1) {
                    free(wf[idx].word);
                    for (int i = idx; i < nw - 1; i++) wf[i] = wf[i + 1];
                    nw--;
                }
            } else if (!strcmp(cmd, "COUNT")) {
                int idx = find(text);
                printf("%d\n", idx != -1 ? wf[idx].count : 0);
            }
        } else if (!strcmp(cmd, "TOP")) {
            for (int i = 0; i < nw - 1; i++) {
                for (int j = 0; j < nw - i - 1; j++) {
                    if (wf[j].count < wf[j+1].count) {
                        WF t = wf[j]; wf[j] = wf[j+1]; wf[j+1] = t;
                    }
                }
            }
            for (int i = 0; i < nw; i++) printf("%s %d\n", wf[i].word, wf[i].count);
        }
    }
    for (int i = 0; i < nw; i++) free(wf[i].word);
    return 0;
}