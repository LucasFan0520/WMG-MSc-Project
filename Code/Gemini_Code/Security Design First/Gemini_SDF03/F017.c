// F017.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} KeyValue;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    KeyValue *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread;
        while ((fread = getline(&fline, &flen, f)) != -1) {
            if (fread > 0 && fline[fread - 1] == '\n') {
                fline[fread - 1] = '\0';
            }
            char *eq = strchr(fline, '=');
            if (!eq) continue;
            *eq = '\0';
            char *key = fline;
            char *val = eq + 1;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                KeyValue *tmp = realloc(arr, capacity * sizeof(KeyValue));
                if (!tmp) exit(1);
                arr = tmp;
            }
            arr[count].key = strdup(key);
            arr[count].value = strdup(val);
            if (!arr[count].key || !arr[count].value) exit(1);
            count++;
        }
        free(fline);
        fclose(f);
    }
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
        if (strncmp(line, "SET ", 4) == 0) {
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
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].value);
                    arr[i].value = strdup(val);
                    if (!arr[i].value) exit(1);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    KeyValue *tmp = realloc(arr, capacity * sizeof(KeyValue));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].key = strdup(key);
                arr[count].value = strdup(val);
                if (!arr[count].key || !arr[count].value) exit(1);
                count++;
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            while (*key == ' ') key++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].key);
                    free(arr[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') key++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    printf("%s\n", arr[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", arr[i].key, arr[i].value);
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
