// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} ConfigPair;

char *read_line_from_file(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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

char *read_line() {
    return read_line_from_file(stdin);
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    ConfigPair *arr = NULL;
    int count = 0;
    if (f) {
        while (1) {
            char *line = read_line_from_file(f);
            if (!line) break;
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                arr = realloc(arr, (count + 1) * sizeof(ConfigPair));
                arr[count].key = strdup(line);
                arr[count].value = strdup(eq + 1);
                count++;
            }
            free(line);
        }
        fclose(f);
    }
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
                    arr = realloc(arr, (count + 1) * sizeof(ConfigPair));
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
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    printf("%s\n", arr[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", arr[i].key, arr[i].value);
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
