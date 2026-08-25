// F014.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    int l_count;
} Item;

int main(void) {
    Item *items = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *note = space + 1;
                Item *tmp = realloc(items, sizeof(Item) * (count + 1));
                if (tmp) {
                    items = tmp;
                    items[count].name = strdup(name);
                    items[count].note = strdup(note);
                    items[count].labels = NULL;
                    items[count].l_count = 0;
                    if (items[count].name && items[count].note) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *p = line + 6;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *label = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        char **l_tmp = realloc(items[i].labels, sizeof(char *) * (items[i].l_count + 1));
                        if (l_tmp) {
                            items[i].labels = l_tmp;
                            items[i].labels[items[i].l_count] = strdup(label);
                            if (items[i].labels[items[i].l_count]) {
                                items[i].l_count++;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *p = line + 8;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *label = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        int idx = -1;
                        for (int j = 0; j < items[i].l_count; j++) {
                            if (strcmp(items[i].labels[j], label) == 0) {
                                idx = j;
                                break;
                            }
                        }
                        if (idx != -1) {
                            free(items[i].labels[idx]);
                            for (int j = idx; j < items[i].l_count - 1; j++) {
                                items[i].labels[j] = items[i].labels[j + 1];
                            }
                            items[i].l_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(items[found].name);
                free(items[found].note);
                for (int j = 0; j < items[found].l_count; j++) {
                    free(items[found].labels[j]);
                }
                free(items[found].labels);
                for (int i = found; i < count - 1; i++) {
                    items[i] = items[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s", items[i].name, items[i].note);
                    for (int j = 0; j < items[i].l_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (int j = 0; j < items[i].l_count; j++) {
                    printf(" %s", items[i].labels[j]);
                }
                printf("\n");
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].l_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    free(line);
    return 0;
}
