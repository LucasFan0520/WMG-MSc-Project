// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

char *read_line() {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(stdin)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            buf = realloc(buf, cap);
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Variable *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val = p + 1;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        free(arr[i].value);
                        arr[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    arr = realloc(arr, (count + 1) * sizeof(Variable));
                    arr[count].key = strdup(key);
                    arr[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].key);
                    free(arr[i].value);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            char *text = line + 7;
            while (*text) {
                if (*text == '{') {
                    char *end = strchr(text, '}');
                    if (end) {
                        *end = '\0';
                        char *key = text + 1;
                        char *val = NULL;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(arr[i].key, key) == 0) {
                                val = arr[i].value;
                                break;
                            }
                        }
                        if (val) {
                            printf("%s", val);
                        } else {
                            printf("{%s}", key);
                        }
                        text = end + 1;
                    } else {
                        putchar(*text);
                        text++;
                    }
                } else {
                    putchar(*text);
                    text++;
                }
            }
            putchar('\n');
        } else if (strcmp(line, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].key, arr[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].key);
        free(arr[i].value);
    }
    free(arr);
    return 0;
}
