// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int lcount;
} Item;

Item *items = NULL;
int icount = 0;

int find_item(const char *name) {
    for (int i = 0; i < icount; i++) {
        if (strcmp(items[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ITEM") == 0) {
            char name[100], note[1000];
            scanf("%s", name);
            scanf(" %[^\n]", note);
            items = realloc(items, sizeof(Item) * (icount + 1));
            items[icount].name = strdup(name);
            items[icount].note = strdup(note);
            items[icount].labels = NULL;
            items[icount].lcount = 0;
            icount++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            char name[100], label[100];
            scanf("%s %s", name, label);
            int idx = find_item(name);
            if (idx != -1) {
                items[idx].labels = realloc(items[idx].labels, sizeof(char*) * (items[idx].lcount + 1));
                items[idx].labels[items[idx].lcount++] = strdup(label);
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char name[100], label[100];
            scanf("%s %s", name, label);
            int idx = find_item(name);
            if (idx != -1) {
                for (int i = 0; i < items[idx].lcount; i++) {
                    if (strcmp(items[idx].labels[i], label) == 0) {
                        free(items[idx].labels[i]);
                        for (int j = i; j < items[idx].lcount - 1; j++) {
                            items[idx].labels[j] = items[idx].labels[j+1];
                        }
                        items[idx].lcount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                free(items[idx].name);
                free(items[idx].note);
                for (int i = 0; i < items[idx].lcount; i++) free(items[idx].labels[i]);
                free(items[idx].labels);
                for (int j = idx; j < icount - 1; j++) items[j] = items[j+1];
                icount--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                printf("%s %s", items[idx].name, items[idx].note);
                for (int i = 0; i < items[idx].lcount; i++) {
                    printf(" %s", items[idx].labels[i]);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < icount; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].lcount; j++) {
                    printf(" %s", items[i].labels[j]);
                }
                printf("\n");
            }
        }
    }
    for (int i = 0; i < icount; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].lcount; j++) free(items[i].labels[j]);
        free(items[i].labels);
    }
    free(items);
    return 0;
}