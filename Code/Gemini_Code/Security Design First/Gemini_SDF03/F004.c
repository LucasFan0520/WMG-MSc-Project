// F004.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

int main(void) {
    Variable *arr = NULL;
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
                    Variable *tmp = realloc(arr, capacity * sizeof(Variable));
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
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            char *text = line + 7;
            while (*text == ' ') text++;
            char *t = text;
            while (*t) {
                if (*t == '{') {
                    char *end = strchr(t, '}');
                    if (end) {
                        size_t klen = end - t - 1;
                        char *k = malloc(klen + 1);
                        if (!k) exit(1);
                        memcpy(k, t + 1, klen);
                        k[klen] = '\0';
                        int v_found = 0;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(arr[i].key, k) == 0) {
                                printf("%s", arr[i].value);
                                v_found = 1;
                                break;
                            }
                        }
                        if (!v_found) {
                            fwrite(t, 1, end - t + 1, stdout);
                        }
                        free(k);
                        t = end + 1;
                    } else {
                        putchar(*t);
                        t++;
                    }
                } else {
                    putchar(*t);
                    t++;
                }
            }
            putchar('\n');
        } else if (strcmp(line, "VARS") == 0) {
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
