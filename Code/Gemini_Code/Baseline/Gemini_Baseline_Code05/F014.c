// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int label_count;
    int label_capacity;
} Item;

Item *items = NULL;
int item_count = 0;
int item_capacity = 0;

int find_item(const char *name) {
    for (int i = 0; i < item_count; i++) {
        if (strcmp(items[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char *p1 = line + 5;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                if (item_count >= item_capacity) {
                    item_capacity = item_capacity == 0 ? 4 : item_capacity * 2;
                    items = realloc(items, item_capacity * sizeof(Item));
                }
                items[item_count].name = strdup(p1);
                items[item_count].note = strdup(p2 + 1);
                items[item_count].labels = NULL;
                items[item_count].label_count = 0;
                items[item_count].label_capacity = 0;
                item_count++;
            }
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *p1 = line + 6;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int idx = find_item(p1);
                if (idx != -1) {
                    Item *it = &items[idx];
                    if (it->label_count >= it->label_capacity) {
                        it->label_capacity = it->label_capacity == 0 ? 4 : it->label_capacity * 2;
                        it->labels = realloc(it->labels, it->label_capacity * sizeof(char *));
                    }
                    it->labels[it->label_count] = strdup(p2 + 1);
                    it->label_count++;
                }
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *p1 = line + 8;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int idx = find_item(p1);
                if (idx != -1) {
                    Item *it = &items[idx];
                    for (int i = 0; i < it->label_count; i++) {
                        if (strcmp(it->labels[i], p2 + 1) == 0) {
                            free(it->labels[i]);
                            for (int j = i; j < it->label_count - 1; j++) {
                                it->labels[j] = it->labels[j + 1];
                            }
                            it->label_count--;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int idx = find_item(name);
            if (idx != -1) {
                free(items[idx].name);
                free(items[idx].note);
                for (int i = 0; i < items[idx].label_count; i++) {
                    free(items[idx].labels[i]);
                }
                free(items[idx].labels);
                for (int i = idx; i < item_count - 1; i++) {
                    items[i] = items[i + 1];
                }
                item_count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            int idx = find_item(name);
            if (idx != -1) {
                printf("%s %s", items[idx].name, items[idx].note);
                for (int i = 0; i < items[idx].label_count; i++) {
                    printf(" %s", items[idx].labels[i]);
                }
                printf("\n");
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < item_count; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].label_count; j++) {
                    printf(" %s", items[i].labels[j]);
                }
                printf("\n");
            }
        }
    }
    for (int i = 0; i < item_count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].label_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
