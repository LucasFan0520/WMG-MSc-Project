// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char title[100];
    char note[1000];
    int done;
} Task;

typedef struct {
    char name[100];
    Task tasks[1000];
    int cnt;
} Proj;

Proj projs[1000];
int pn = 0;

int findProj(char *name) {
    for (int i = 0; i < pn; i++) {
        if (!strcmp(projs[i].name, name)) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[100], title[100], note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "PROJECT")) {
            scanf("%s", name);
            strcpy(projs[pn].name, name);
            projs[pn].cnt = 0;
            pn++;
        } else if (!strcmp(cmd, "TASK")) {
            scanf("%s %s", name, title);
            fgets(note, 1000, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            int i = findProj(name);
            if (i != -1) {
                strcpy(projs[i].tasks[projs[i].cnt].title, title);
                strcpy(projs[i].tasks[projs[i].cnt].note, note);
                projs[i].tasks[projs[i].cnt].done = 0;
                projs[i].cnt++;
            }
        } else if (!strcmp(cmd, "MOVETASK")) {
            char from[100], to[100];
            scanf("%s %s %s", from, to, title);
            int i1 = findProj(from);
            int i2 = findProj(to);
            if (i1 != -1 && i2 != -1) {
                for (int j = 0; j < projs[i1].cnt; j++) {
                    if (!strcmp(projs[i1].tasks[j].title, title)) {
                        projs[i2].tasks[projs[i2].cnt] = projs[i1].tasks[j];
                        projs[i2].cnt++;
                        for (int k = j; k < projs[i1].cnt - 1; k++) {
                            projs[i1].tasks[k] = projs[i1].tasks[k + 1];
                        }
                        projs[i1].cnt--;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DONETASK")) {
            scanf("%s %s", name, title);
            int i = findProj(name);
            if (i != -1) {
                for (int j = 0; j < projs[i].cnt; j++) {
                    if (!strcmp(projs[i].tasks[j].title, title)) {
                        projs[i].tasks[j].done = 1;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETEPROJECT")) {
            scanf("%s", name);
            int i = findProj(name);
            if (i != -1) {
                for (int j = i; j < pn - 1; j++) {
                    projs[j] = projs[j + 1];
                }
                pn--;
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < pn; i++) {
                printf("%s\n", projs[i].name);
                for (int j = 0; j < projs[i].cnt; j++) {
                    printf("%s %s %s\n", projs[i].tasks[j].title, projs[i].tasks[j].done ? "DONE" : "TODO", projs[i].tasks[j].note);
                }
            }
        }
    }
    return 0;
}