// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 1000
#define MAX_LABELS 100
#define MAX_LEN 1000

typedef struct {
    char name[100];
    char note[MAX_LEN];
    char labels[MAX_LABELS][100];
    int lcount;
} Item;

Item items[MAX_ITEMS];
int count = 0;

int find_item(const char *name) {
    for (int i = 0; i < count; i++) if (strcmp(items[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], note[MAX_LEN], label[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ITEM") == 0) {
            scanf("%s", name);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (find_item(name) == -1 && count < MAX_ITEMS) {
                strcpy(items[count].name, name);
                strcpy(items[count].note, note);
                items[count].lcount = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            scanf("%s %s", name, label);
            int idx = find_item(name);
            if (idx != -1 && items[idx].lcount < MAX_LABELS) {
                strcpy(items[idx].labels[items[idx].lcount++], label);
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            scanf("%s %s", name, label);
            int idx = find_item(name);
            if (idx != -1) {
                for (int i = 0; i < items[idx].lcount; i++) {
                    if (strcmp(items[idx].labels[i], label) == 0) {
                        for (int j = i; j < items[idx].lcount - 1; j++)
                            strcpy(items[idx].labels[j], items[idx].labels[j + 1]);
                        items[idx].lcount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) items[i] = items[i + 1];
                count--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_item(name);
            if (idx != -1) {
                printf("%s %s", items[idx].name, items[idx].note);
                for (int i = 0; i < items[idx].lcount; i++) printf(" %s", items[idx].labels[i]);
                printf("\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].lcount; j++) printf(" %s", items[i].labels[j]);
                printf("\n");
            }
        }
    }
    return 0;
}