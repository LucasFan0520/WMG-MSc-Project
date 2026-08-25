// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *email; char *phone; char *note; } Con;
Con c[1000]; int nc = 0;

int find(char *name) {
    for (int i = 0; i < nc; i++) if (!strcmp(c[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], email[100], phone[100], note[10000], rm[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s %s", name, email, phone);
            c[nc].name = strdup(name); c[nc].email = strdup(email);
            c[nc].phone = strdup(phone); c[nc++].note = strdup("");
        } else if (!strcmp(cmd, "NOTE")) {
            scanf("%s", name); fgets(note, sizeof(note), stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            int idx = find(name);
            if (idx != -1) { free(c[idx].note); c[idx].note = strdup(note); }
        } else if (!strcmp(cmd, "MERGE")) {
            scanf("%s %s", name, rm);
            int i1 = find(name), i2 = find(rm);
            if (i1 != -1 && i2 != -1) {
                char *nn = malloc(strlen(c[i1].note) + strlen(c[i2].note) + 2);
                sprintf(nn, "%s %s", c[i1].note, c[i2].note);
                free(c[i1].note); c[i1].note = nn;
                free(c[i2].name); free(c[i2].email); free(c[i2].phone); free(c[i2].note);
                for (int i = i2; i < nc - 1; i++) c[i] = c[i + 1];
                nc--;
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name); int idx = find(name);
            if (idx != -1) {
                free(c[idx].name); free(c[idx].email); free(c[idx].phone); free(c[idx].note);
                for (int i = idx; i < nc - 1; i++) c[i] = c[i + 1];
                nc--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", name); int idx = find(name);
            if (idx != -1) printf("%s %s %s %s\n", c[idx].name, c[idx].email, c[idx].phone, c[idx].note);
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < nc; i++) printf("%s %s %s %s\n", c[i].name, c[i].email, c[i].phone, c[i].note);
        }
    }
    for (int i = 0; i < nc; i++) { free(c[i].name); free(c[i].email); free(c[i].phone); free(c[i].note); }
    return 0;
}