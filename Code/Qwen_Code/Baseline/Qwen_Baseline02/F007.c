// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int time; char *label; char *note; } Act;
Act a[10000]; int na = 0;

int main() {
    char cmd[20], label[100], note[10000]; int time;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%d %s", &time, label); fgets(note, sizeof(note), stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            a[na].time = time; a[na].label = strdup(label); a[na++].note = strdup(note);
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%d %s", &time, label);
            for (int i = 0; i < na; i++) {
                if (a[i].time == time && !strcmp(a[i].label, label)) {
                    free(a[i].label); free(a[i].note);
                    for (int j = i; j < na - 1; j++) a[j] = a[j + 1];
                    na--; i--;
                }
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", label);
            for (int i = 0; i < na; i++) {
                if (!strcmp(a[i].label, label)) {
                    printf("%d %s %s\n", a[i].time, a[i].label, a[i].note); break;
                }
            }
        } else if (!strcmp(cmd, "SORT")) {
            for (int i = 0; i < na - 1; i++) {
                for (int j = 0; j < na - i - 1; j++) {
                    int swap = 0;
                    if (a[j].time > a[j+1].time) swap = 1;
                    else if (a[j].time == a[j+1].time && strcmp(a[j].label, a[j+1].label) > 0) swap = 1;
                    if (swap) { Act t = a[j]; a[j] = a[j+1]; a[j+1] = t; }
                }
            }
            for (int i = 0; i < na; i++) printf("%d %s %s\n", a[i].time, a[i].label, a[i].note);
        } else if (!strcmp(cmd, "CLEAR")) {
            for (int i = 0; i < na; i++) { free(a[i].label); free(a[i].note); }
            na = 0;
        }
    }
    for (int i = 0; i < na; i++) { free(a[i].label); free(a[i].note); }
    return 0;
}