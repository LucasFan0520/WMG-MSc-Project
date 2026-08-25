// F014.c
#define _GNU_SOURCE
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ITEM") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            items = realloc(items, (count + 1) * sizeof(Item));
            if (!items) return 1;
            items[count].name = strdup(name);
            items[count].note = strdup(note);
            items[count].labels = NULL;
            items[count].label_count = 0;
            count++;
        } else if (strcmp(cmd, "LABEL") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *lbl = space2 + 1;
                while (*lbl == ' ') lbl++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        int lc = items[i].label_count;
                        items[i].labels = realloc(items[i].labels, (lc + 1) * sizeof(char *));
                        if (!items[i].labels) return 1;
                        items[i].labels[lc] = strdup(lbl);
                        items[i].label_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *lbl = space2 + 1;
                while (*lbl == ' ') lbl++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        for (int j = 0; j < items[i].label_count; j++) {
                            if (strcmp(items[i].labels[j], lbl) == 0) {
                                free(items[i].labels[j]);
                                for (int k = j; k < items[i].label_count - 1; k++) {
                                    items[i].labels[k] = items[i].labels[k + 1];
                                }
                                items[i].label_count--;
                                items[i].labels = realloc(items[i].labels, items[i].label_count * sizeof(char *));
                                if (items[i].label_count > 0 && !items[i].labels) return 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, args) == 0) {
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
                    items = realloc(items, count * sizeof(Item));
                    if (count > 0 && !items) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, args) == 0) {
                    printf("%s %s", items[i].name, items[i].note);
                    for (int j = 0; j < items[i].label_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].label_count; j++) {
                    printf(" %s", items[i].labels[j]);
                }
                putchar('\n');
            }
        }
    }

    free(line);
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
