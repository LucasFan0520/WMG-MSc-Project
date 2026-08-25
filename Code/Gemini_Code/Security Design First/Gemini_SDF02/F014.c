/* F014.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
    char **labels;
    size_t l_count;
    size_t l_capacity;
} Item;

int main(void) {
    Item *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;

        if (strcmp(cmd, "ITEM") == 0) {
            char *name_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note_str = p;

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Item *temp = realloc(items, new_cap * sizeof(Item));
                if (!temp) break;
                items = temp;
                capacity = new_cap;
            }
            items[count].name = strdup(name_str);
            items[count].note = strdup(note_str);
            items[count].labels = NULL;
            items[count].l_count = 0;
            items[count].l_capacity = 0;
            if (items[count].name && items[count].note) {
                count++;
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label_str = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name_str) == 0) {
                    if (items[i].l_count >= items[i].l_capacity) {
                        size_t new_cap = items[i].l_capacity == 0 ? 4 : items[i].l_capacity * 2;
                        char **temp = realloc(items[i].labels, new_cap * sizeof(char *));
                        if (!temp) break;
                        items[i].labels = temp;
                        items[i].l_capacity = new_cap;
                    }
                    items[i].labels[items[i].l_count] = strdup(label_str);
                    if (items[i].labels[items[i].l_count]) {
                        items[i].l_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *label_str = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name_str) == 0) {
                    for (size_t j = 0; j < items[i].l_count; j++) {
                        if (strcmp(items[i].labels[j], label_str) == 0) {
                            free(items[i].labels[j]);
                            for (size_t k = j; k < items[i].l_count - 1; k++) {
                                items[i].labels[k] = items[i].labels[k + 1];
                            }
                            items[i].l_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name_str) == 0) {
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = 0; j < items[i].l_count; j++) {
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
            char *name_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name_str) == 0) {
                    printf("%s %s", items[i].name, items[i].note);
                    for (size_t j = 0; j < items[i].l_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (size_t j = 0; j < items[i].l_count; j++) {
                    printf(" %s", items[i].labels[j]);
                }
                printf("\n");
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].note);
        for (size_t j = 0; j < items[i].l_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
    }
    free(items);
    return 0;
}
