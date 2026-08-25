// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} CleaningRecord;

int main(void) {
    CleaningRecord *items = NULL;
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
            char *key = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *value = p;

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                CleaningRecord *new_items = realloc(items, capacity * sizeof(CleaningRecord));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].key = strdup(key);
            items[count].value = strdup(value);
            if (!items[count].key || !items[count].value) exit(1);
            count++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (size_t j = i + 1; j < count; ) {
                    if (strcmp(items[i].key, items[j].key) == 0) {
                        free(items[j].key);
                        free(items[j].value);
                        for (size_t k = j; k < count - 1; k++) {
                            items[k] = items[k + 1];
                        }
                        count--;
                    } else {
                        j++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; ) {
                if (strcmp(items[i].key, key) == 0) {
                    free(items[i].key);
                    free(items[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    printf("%s\n", items[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].key, items[i].value);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].key);
        free(items[i].value);
    }
    free(items);
    free(line);
    return 0;
}
