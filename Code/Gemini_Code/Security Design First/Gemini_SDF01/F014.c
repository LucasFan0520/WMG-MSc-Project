// F014.c
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
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "ITEM") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found != -1) {
                free(items[found].note);
                items[found].note = strdup(note);
                if (!items[found].note) exit(1);
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Item *new_items = realloc(items, capacity * sizeof(Item));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].name = strdup(name);
                items[count].note = strdup(note);
                if (!items[count].name || !items[count].note) exit(1);
                items[count].labels = NULL;
                items[count].label_count = 0;
                items[count].label_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    Item *it = &items[i];
                    if (it->label_count >= it->label_capacity) {
                        it->label_capacity = it->label_capacity == 0 ? 4 : it->label_capacity * 2;
                        char **nl = realloc(it->labels, it->label_capacity * sizeof(char *));
                        if (!nl) exit(1);
                        it->labels = nl;
                    }
                    it->labels[it->label_count] = strdup(label);
                    if (!it->labels[it->label_count]) exit(1);
                    it->label_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *label = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    Item *it = &items[i];
                    for (size_t j = 0; j < it->label_count; j++) {
                        if (strcmp(it->labels[j], label) == 0) {
                            free(it->labels[j]);
                            for (size_t k = j; k < it->label_count - 1; k++) {
                                it->labels[k] = it->labels[k + 1];
                            }
                            it->label_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

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
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s Labels:", items[i].name, items[i].note);
                    for (size_t j = 0; j < items[i].label_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
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
