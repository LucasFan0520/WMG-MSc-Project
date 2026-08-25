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
    Item *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
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
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                note = p;
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Item *tmp = realloc(arr, capacity * sizeof(Item));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].name = strdup(name);
                arr[count].note = strdup(note);
                arr[count].labels = NULL;
                arr[count].label_count = 0;
                arr[count].label_capacity = 0;
                if (!arr[count].name || !arr[count].note) exit(1);
                count++;
            }
        } else if (strncmp(line, "LABEL ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *label = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                label = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    Item *item = &arr[i];
                    if (item->label_count >= item->label_capacity) {
                        item->label_capacity = item->label_capacity == 0 ? 4 : item->label_capacity * 2;
                        char **tmp = realloc(item->labels, item->label_capacity * sizeof(char *));
                        if (!tmp) exit(1);
                        item->labels = tmp;
                    }
                    item->labels[item->label_count] = strdup(label);
                    if (!item->labels[item->label_count]) exit(1);
                    item->label_count++;
                    break;
                }
            }
        } else if (strncmp(line, "UNLABEL ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *label = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                label = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    Item *item = &arr[i];
                    for (size_t j = 0; j < item->label_count; j++) {
                        if (strcmp(item->labels[j], label) == 0) {
                            free(item->labels[j]);
                            for (size_t m = j; m < item->label_count - 1; m++) {
                                item->labels[m] = item->labels[m + 1];
                            }
                            item->label_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (size_t j = 0; j < arr[i].label_count; j++) {
                        free(arr[i].labels[j]);
                    }
                    free(arr[i].labels);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    printf("%s %s Labels:", arr[i].name, arr[i].note);
                    for (size_t j = 0; j < arr[i].label_count; j++) {
                        printf(" %s", arr[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].name);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
        for (size_t j = 0; j < arr[i].label_count; j++) {
            free(arr[i].labels[j]);
        }
        free(arr[i].labels);
    }
    free(arr);
    free(line);
    return 0;
}
