// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Per;
Per q[10000]; int nq = 0;

int find(char *name) {
    for (int i = 0; i < nq; i++) if (!strcmp(q[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], note[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "JOIN") || !strcmp(cmd, "REJOIN")) {
            scanf("%s", name); fgets(note, sizeof(note), stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            q[nq].name = strdup(name); q[nq++].note = strdup(note);
        } else if (!strcmp(cmd, "SERVE")) {
            if (nq > 0) {
                printf("%s\n", q[0].name);
                free(q[0].name); free(q[0].note);
                for (int i = 0; i < nq - 1; i++) q[i] = q[i + 1];
                nq--;
            }
        } else if (!strcmp(cmd, "CANCEL")) {
            scanf("%s", name); int idx = find(name);
            if (idx != -1) {
                free(q[idx].name); free(q[idx].note);
                for (int i = idx; i < nq - 1; i++) q[i] = q[i + 1];
                nq--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", name); int idx = find(name);
            if (idx != -1) printf("%s\n", q[idx].note);
        } else if (!strcmp(cmd, "QUEUE")) {
            for (int i = 0; i < nq; i++) printf("%s\n", q[i].name);
        }
    }
    for (int i = 0; i < nq; i++) { free(q[i].name); free(q[i].note); }
    return 0;
}