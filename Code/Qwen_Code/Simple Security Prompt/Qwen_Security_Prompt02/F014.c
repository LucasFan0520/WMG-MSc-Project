// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *label;
} Label;

typedef struct {
    char *name;
    char *note;
    Label *labels;
    int count;
    int cap;
} Item;

Item *items = NULL;
int total = 0;

int find_name(const char *name) {
    for (int i = 0; i < total; i++) {
        if (strcmp(items[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void add_label(int idx, const char *label) {
    if (items[idx].count == items[idx].cap) {
        items[idx].cap = items[idx].cap ? items[idx].cap * 2 : 4;
        items[idx].labels = realloc(items[idx].labels, sizeof(Label) * items[idx].cap);
    }
    items[idx].labels[items[idx].count++].label = strdup(label);
}

int main() {
    char cmd[20];
    char name[1000];
    char note[10000];
    char label[1000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ITEM") == 0) {
            scanf("%s", name);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = 0;
            int idx = find_name(name);
            if (idx == -1) {
                items = realloc(items, sizeof(Item) * (total + 1));
                items[total].name = strdup(name);
                items[total].note = strdup(note);
                items[total].labels = NULL;
                items[total].count = 0;
                items[total].cap = 0;
                total++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            scanf("%s %s", name, label);
            int idx = find_name(name);
            if (idx != -1) {
                add_label(idx, label);
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            scanf("%s %s", name, label);
            int idx = find_name(name);
            if (idx != -1) {
                for (int i = 0; i < items[idx].count; i++) {
                    if (strcmp(items[idx].labels[i].label, label) == 0) {
                        free(items[idx].labels[i].label);
                        for (int j = i; j < items[idx].count - 1; j++) {
                            items[idx].labels[j] = items[idx].labels[j + 1];
                        }
                        items[idx].count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(items[idx].name);
                free(items[idx].note);
                for (int i = 0; i < items[idx].count; i++) {
                    free(items[idx].labels[i].label);
                }
                free(items[idx].labels);
                for (int i = idx; i < total - 1; i++) {
                    items[i] = items[i + 1];
                }
                total--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                printf("%s %s", items[idx].name, items[idx].note);
                for (int i = 0; i < items[idx].count; i++) {
                    printf(" %s", items[idx].labels[i].label);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < total; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].count; j++) {
                    printf(" %s", items[i].labels[j].label);
                }
                printf("\n");
            }
        }
    }

    for (int i = 0; i < total; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].count; j++) {
            free(items[i].labels[j].label);
        }
        free(items[i].labels);
    }
    free(items);

    return 0;
}