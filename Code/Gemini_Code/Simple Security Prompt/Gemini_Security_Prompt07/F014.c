// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int label_count;
} Item;

int main() {
    Item *items = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && note) {
                items = realloc(items, (count + 1) * sizeof(Item));
                items[count].name = strdup(name);
                items[count].note = strdup(note);
                items[count].labels = NULL;
                items[count].label_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = strtok(NULL, " ");
            char *lbl = strtok(NULL, " ");
            if (name && lbl) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        items[i].labels = realloc(items[i].labels, (items[i].label_count + 1) * sizeof(char *));
                        items[i].labels[items[i].label_count] = strdup(lbl);
                        items[i].label_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = strtok(NULL, " ");
            char *lbl = strtok(NULL, " ");
            if (name && lbl) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        for (int j = 0; j < items[i].label_count; j++) {
                            if (strcmp(items[i].labels[j], lbl) == 0) {
                                free(items[i].labels[j]);
                                for (int k = j; k < items[i].label_count - 1; k++) {
                                    items[i].labels[k] = items[i].labels[k + 1];
                                }
                                items[i].label_count--;
                                if (items[i].label_count == 0) {
                                    free(items[i].labels);
                                    items[i].labels = NULL;
                                } else {
                                    items[i].labels = realloc(items[i].labels, items[i].label_count * sizeof(char *));
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        free(items[i].name);
                        free(items[i].note);
                        for (int j = 0; j < items[i].label_count; j++) {
                            free(items[i].labels[j]);
                        }
                        free(items[i].labels);
                        for (int j = i; j < count - 1; j++) {
                            items[j] = items[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(items);
                    items = NULL;
                } else {
                    items = realloc(items, count * sizeof(Item));
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        printf("%s %s labels:", items[i].name, items[i].note);
                        for (int j = 0; j < items[i].label_count; j++) {
                            printf(" %s", items[i].labels[j]);
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
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
