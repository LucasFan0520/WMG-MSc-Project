// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *note;
    char **labels;
    int label_count;
    int label_capacity;
} Item;

int main(void) {
    Item *items = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ITEM ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note_start = p + 1;
                while (*note_start == ' ') note_start++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Item *new_items = realloc(items, capacity * sizeof(Item));
                    if (new_items) items = new_items;
                }
                if (count < capacity) {
                    items[count].name = strdup(name_start);
                    items[count].note = strdup(note_start);
                    items[count].labels = NULL;
                    items[count].label_count = 0;
                    items[count].label_capacity = 0;
                    count++;
                }
            }
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *label_start = p + 1;
                while (*label_start == ' ') label_start++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name_start) == 0) {
                        Item *it = &items[i];
                        if (it->label_count >= it->label_capacity) {
                            it->label_capacity = it->label_capacity == 0 ? 4 : it->label_capacity * 2;
                            char **new_lbs = realloc(it->labels, it->label_capacity * sizeof(char *));
                            if (new_lbs) it->labels = new_lbs;
                        }
                        if (it->label_count < it->label_capacity) {
                            it->labels[it->label_count] = strdup(label_start);
                            it->label_count++;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *label_start = p + 1;
                while (*label_start == ' ') label_start++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(items[i].name, name_start) == 0) {
                        for (int j = 0; j < items[i].label_count; j++) {
                            if (strcmp(items[i].labels[j], label_start) == 0) {
                                free(items[i].labels[j]);
                                for (int k = j; k < items[i].label_count - 1; k++) {
                                    items[i].labels[k] = items[i].labels[k + 1];
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
            char *name_start = line + 7;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name_start) == 0) {
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
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name_start = line + 5;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].name, name_start) == 0) {
                    printf("%s %s:", items[i].name, items[i].note);
                    for (int j = 0; j < items[i].label_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].note);
            }
        }
        free(line);
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
