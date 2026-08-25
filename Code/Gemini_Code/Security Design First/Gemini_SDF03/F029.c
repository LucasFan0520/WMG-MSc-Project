// F029.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} CleaningRecord;

int main(void) {
    CleaningRecord *arr = NULL;
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            char *val = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                val = p;
            }
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                CleaningRecord *tmp = realloc(arr, capacity * sizeof(CleaningRecord));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].key = strdup(key);
            arr[count].value = strdup(val);
            if (!arr[count].key || !arr[count].value) exit(1);
            count++;
        } else if (strcmp(line, "DEDUP") == 0) {
            for (size_t i = 0; i < count; i++) {
                for (size_t j = i + 1; j < count; ) {
                    if (strcmp(arr[i].key, arr[j].key) == 0) {
                        free(arr[j].key);
                        free(arr[j].value);
                        for (size_t k = j; k < count - 1; k++) {
                            arr[k] = arr[k + 1];
                        }
                        count--;
                    } else {
                        j++;
                    }
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *key = line + 7;
            while (*key == ' ') key++;
            for (size_t i = 0; i < count; ) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].key);
                    free(arr[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *key = line + 5;
            while (*key == ' ') key++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    printf("%s\n", arr[i].value);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].key, arr[i].value);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].key);
        free(arr[i].value);
    }
    free(arr);
    free(line);
    return 0;
}
