// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char *labels[100];
    int lc;
} Item;

Item items[1000];
int ic = 0;

int find_item(char *name) {
    for (int i = 0; i < ic; i++) if (strcmp(items[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ITEM") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *note = rest + strlen(name);
            while (*note == ' ') note++;
            items[ic].name = strdup(name);
            items[ic].note = strdup(note);
            items[ic].lc = 0;
            ic++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[1000], label[1000];
            sscanf(rest, "%s %s", name, label);
            int idx = find_item(name);
            if (idx != -1) {
                items[idx].labels[items[idx].lc++] = strdup(label);
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[1000], label[1000];
            sscanf(rest, "%s %s", name, label);
            int idx = find_item(name);
            if (idx != -1) {
                for (int i = 0; i < items[idx].lc; i++) {
                    if (strcmp(items[idx].labels[i], label) == 0) {
                        free(items[idx].labels[i]);
                        for (int j = i; j < items[idx].lc - 1; j++) items[idx].labels[j] = items[idx].labels[j+1];
                        items[idx].lc--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                free(items[idx].name);
                free(items[idx].note);
                for (int i = 0; i < items[idx].lc; i++) free(items[idx].labels[i]);
                for (int i = idx; i < ic - 1; i++) items[i] = items[i+1];
                ic--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                printf("%s %s", items[idx].name, items[idx].note);
                for (int i = 0; i < items[idx].lc; i++) printf(" %s", items[idx].labels[i]);
                printf("\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < ic; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].lc; j++) printf(" %s", items[i].labels[j]);
                printf("\n");
            }
        }
    }
    for (int i = 0; i < ic; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].lc; j++) free(items[i].labels[j]);
    }
    return 0;
}