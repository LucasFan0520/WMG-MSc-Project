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
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *note = space + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Item *next = realloc(items, capacity * sizeof(Item));
                    if (!next) break;
                    items = next;
                }
                items[count].name = strdup(name);
                items[count].note = strdup(note);
                items[count].labels = NULL;
                items[count].label_count = 0;
                items[count].label_capacity = 0;
                count++;
            }
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *args = line + 6;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *label = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        if (items[i].label_count >= items[i].label_capacity) {
                            items[i].label_capacity = items[i].label_capacity == 0 ? 4 : items[i].label_capacity * 2;
                            char **nxt = realloc(items[i].labels, items[i].label_capacity * sizeof(char *));
                            if (!nxt) break;
                            items[i].labels = nxt;
                        }
                        items[i].labels[items[i].label_count++] = strdup(label);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *args = line + 8;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *label = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name) == 0) {
                        for (size_t k = 0; k < items[i].label_count; k++) {
                            if (strcmp(items[i].labels[k], label) == 0) {
                                free(items[i].labels[k]);
                                for (size_t j = k; j < items[i].label_count - 1; j++) {
                                    items[i].labels[j] = items[i].labels[j + 1];
                                }
                                items[i].label_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t k = 0; k < items[i].label_count; k++) {
                        free(items[i].labels[k]);
                    }
                    free(items[i].labels);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s", items[i].name, items[i].note);
                    for (size_t k = 0; k < items[i].label_count; k++) {
                        printf(" %s", items[i].labels[k]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (size_t k = 0; k < items[i].label_count; k++) {
                    printf(" %s", items[i].labels[k]);
                }
                printf("\n");
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (size_t k = 0; k < items[i].label_count; k++) {
            free(items[i].labels[k]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
