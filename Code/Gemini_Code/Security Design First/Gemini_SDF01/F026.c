// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *category;
    char *text;
} Note;

int main(void) {
    Note *items = NULL;
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

        if (strcmp(cmd, "ADD") == 0) {
            char *category = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *text = p;

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Note *new_items = realloc(items, capacity * sizeof(Note));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].category = strdup(category);
            items[count].text = strdup(text);
            if (!items[count].category || !items[count].text) exit(1);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *category = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; ) {
                if (strcmp(items[i].category, category) == 0) {
                    free(items[i].category);
                    free(items[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *category = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; ) {
                if (strcmp(items[i].category, category) != 0) {
                    free(items[i].category);
                    free(items[i].text);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strstr(items[i].text, word) != NULL) {
                    printf("[%s] %s\n", items[i].category, items[i].text);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("[%s] %s\n", items[i].category, items[i].text);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].category);
        free(items[i].text);
    }
    free(items);
    free(line);
    return 0;
}
