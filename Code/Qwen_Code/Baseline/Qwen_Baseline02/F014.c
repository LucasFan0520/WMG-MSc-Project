// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; char **labels; int nl; } Item;
Item items[1000]; int ni = 0;

int find(char *name) {
    for (int i = 0; i < ni; i++) if (!strcmp(items[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], note[1000], label[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ITEM")) {
            scanf("%s", name); fgets(note, sizeof(note), stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            items[ni].name = strdup(name); items[ni].note = strdup(note);
            items[ni].labels = NULL; items[ni++].nl = 0;
        } else if (!strcmp(cmd, "LABEL")) {
            scanf("%s %s", name, label); int idx = find(name);
            if (idx != -1) {
                items[idx].labels = realloc(items[idx].labels, sizeof(char*) * (items[idx].nl + 1));
                items[idx].labels[items[idx].nl++] = strdup(label);
            }
        } else if (!strcmp(cmd, "UNLABEL")) {
            scanf("%s %s", name, label); int idx = find(name);
            if (idx != -1) {
                for (int i = 0; i < items[idx].nl; i++) {
                    if (!strcmp(items[idx].labels[i], label)) {
                        free(items[idx].labels[i]);
                        for (int j = i; j < items[idx].nl - 1; j++) items[idx].labels[j] = items[idx].labels[j + 1];
                        items[idx].nl--; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", name); int idx = find(name);
            if (idx != -1) {
                free(items[idx].name); free(items[idx].note);
                for (int i = 0; i < items[idx].nl; i++) free(items[idx].labels[i]);
                free(items[idx].labels);
                for (int i = idx; i < ni - 1; i++) items[i] = items[i + 1];
                ni--;
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", name); int idx = find(name);
            if (idx != -1) {
                printf("%s %s", items[idx].name, items[idx].note);
                for (int i = 0; i < items[idx].nl; i++) printf(" %s", items[idx].labels[i]);
                printf("\n");
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < ni; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].nl; j++) printf(" %s", items[i].labels[j]);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < ni; i++) {
        free(items[i].name); free(items[i].note);
        for (int j = 0; j < items[i].nl; j++) free(items[i].labels[j]);
        free(items[i].labels);
    }
    return 0;
}