// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    int label_count;
} Item;

int main() {
    Item *items = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "REPORT") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", items[i].name, items[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ITEM") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *note = p2 + 1;
                items = realloc(items, sizeof(Item) * (count + 1));
                items[count].name = strdup(name);
                items[count].note = strdup(note);
                items[count].labels = NULL;
                items[count].label_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *label = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        int lc = items[i].label_count;
                        items[i].labels = realloc(items[i].labels, sizeof(char*) * (lc + 1));
                        items[i].labels[lc] = strdup(label);
                        items[i].label_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *label = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        for (int j = 0; j < items[i].label_count; j++) {
                            if (strcmp(items[i].labels[j], label) == 0) {
                                free(items[i].labels[j]);
                                for (int k = j; k < items[i].label_count - 1; k++) {
                                    items[i].labels[k] = items[i].labels[k + 1];
                                }
                                items[i].label_count--;
                                if (items[i].label_count == 0) {
                                    free(items[i].labels);
                                    items[i].labels = NULL;
                                } else {
                                    items[i].labels = realloc(items[i].labels, sizeof(char*) * items[i].label_count);
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (int j = 0; j < items[i].label_count; j++) free(items[i].labels[j]);
                    free(items[i].labels);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    if (count == 0) { free(items); items = NULL; }
                    else { items = realloc(items, sizeof(Item) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s [", items[i].name, items[i].note);
                    for (int j = 0; j < items[i].label_count; j++) {
                        printf("%s%s", items[i].labels[j], (j == items[i].label_count - 1) ? "" : ", ");
                    }
                    printf("]\n");
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (int j = 0; j < items[i].label_count; j++) free(items[i].labels[j]);
        free(items[i].labels);
    }
    free(items);
    return 0;
}
