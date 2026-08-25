// F014.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    size_t label_count;
    size_t label_capacity;
} Item;

int main(void) {
    Item *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ITEM") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;

            if (*name == '\0') continue;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Item *new_items = realloc(items, new_cap * sizeof(Item));
                if (!new_items) continue;
                items = new_items;
                capacity = new_cap;
            }
            char *n = strdup(name);
            char *nt = strdup(note);
            if (n && nt) {
                items[count].name = n;
                items[count].note = nt;
                items[count].labels = NULL;
                items[count].label_count = 0;
                items[count].label_capacity = 0;
                count++;
            } else {
                free(n);
                free(nt);
            }
        } else if (strcmp(cmd, "LABEL") == 0 || strcmp(cmd, "UNLABEL") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*name == '\0' || *label == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (strcmp(cmd, "LABEL") == 0) {
                        if (items[i].label_count >= items[i].label_capacity) {
                            size_t new_cap = items[i].label_capacity == 0 ? 4 : items[i].label_capacity * 2;
                            char **new_labs = realloc(items[i].labels, new_cap * sizeof(char *));
                            if (!new_labs) continue;
                            items[i].labels = new_labs;
                            items[i].label_capacity = new_cap;
                        }
                        char *l = strdup(label);
                        if (l) {
                            items[i].labels[items[i].label_count] = l;
                            items[i].label_count++;
                        }
                    } else {
                        for (size_t j = 0; j < items[i].label_count; j++) {
                            if (strcmp(items[i].labels[j], label) == 0) {
                                free(items[i].labels[j]);
                                for (size_t k = j; k < items[i].label_count - 1; k++) {
                                    items[i].labels[k] = items[i].labels[k + 1];
                                }
                                items[i].label_count--;
                                break;
                            }
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = 0; j < items[i].label_count; j++) {
                        free(items[i].labels[j]);
                    }
                    free(items[i].labels);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s Labels:", items[i].name, items[i].note);
                    for (size_t j = 0; j < items[i].label_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].name);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (size_t j = 0; j < items[i].label_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    free(line);
    return 0;
}
