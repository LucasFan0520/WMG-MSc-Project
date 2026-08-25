// F014.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Item;

typedef struct {
    char *name;
    char *label;
} LabelAssign;

Item *items = NULL;
int item_count = 0;
int item_cap = 0;

LabelAssign *labels = NULL;
int label_count = 0;
int label_cap = 0;

int find_item(const char *name) {
    for (int i = 0; i < item_count; i++) {
        if (strcmp(items[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ITEM") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *note = space2 + 1;
                    if (item_count >= item_cap) {
                        item_cap = item_cap == 0 ? 4 : item_cap * 2;
                        Item *tmp = realloc(items, item_cap * sizeof(Item));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        items = tmp;
                    }
                    items[item_count].name = strdup(arg1);
                    items[item_count].note = strdup(note);
                    if (items[item_count].name && items[item_count].note) {
                        item_count++;
                    }
                }
            } else if (strcmp(cmd, "LABEL") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *lbl = space2 + 1;
                    if (label_count >= label_cap) {
                        label_cap = label_cap == 0 ? 4 : label_cap * 2;
                        LabelAssign *tmp = realloc(labels, label_cap * sizeof(LabelAssign));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        labels = tmp;
                    }
                    labels[label_count].name = strdup(arg1);
                    labels[label_count].label = strdup(lbl);
                    if (labels[label_count].name && labels[label_count].label) {
                        label_count++;
                    }
                }
            } else if (strcmp(cmd, "UNLABEL") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *lbl = space2 + 1;
                    for (int i = 0; i < label_count; i++) {
                        if (strcmp(labels[i].name, arg1) == 0 && strcmp(labels[i].label, lbl) == 0) {
                            free(labels[i].name);
                            free(labels[i].label);
                            for (int j = i; j < label_count - 1; j++) {
                                labels[j] = labels[j + 1];
                            }
                            label_count--;
                            break;
                        }
                    }
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                int idx = find_item(arg1);
                if (idx != -1) {
                    free(items[idx].name);
                    free(items[idx].note);
                    for (int i = idx; i < item_count - 1; i++) {
                        items[i] = items[i + 1];
                    }
                    item_count--;
                }
                for (int i = 0; i < label_count; ) {
                    if (strcmp(labels[i].name, arg1) == 0) {
                        free(labels[i].name);
                        free(labels[i].label);
                        for (int j = i; j < label_count - 1; j++) {
                            labels[j] = labels[j + 1];
                        }
                        label_count--;
                    } else {
                        i++;
                    }
                }
            } else if (strcmp(cmd, "FIND") == 0) {
                int idx = find_item(arg1);
                if (idx != -1) {
                    printf("%s %s:", items[idx].name, items[idx].note);
                    for (int i = 0; i < label_count; i++) {
                        if (strcmp(labels[i].name, arg1) == 0) {
                            printf(" %s", labels[i].label);
                        }
                    }
                    printf("\n");
                }
            }
        } else {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < item_count; i++) {
                    printf("%s %s\n", items[i].name, items[i].note);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < item_count; i++) {
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
    for (int i = 0; i < label_count; i++) {
        free(labels[i].name);
        free(labels[i].label);
    }
    free(labels);
    return 0;
}