// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *value;
} ValueNode;

typedef struct {
    char *key;
    ValueNode *vals;
    int val_count;
} KeyNode;

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
    KeyNode *arr = NULL;
    int count = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val = p + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found == -1) {
                    arr = realloc(arr, (count + 1) * sizeof(KeyNode));
                    arr[count].key = strdup(key);
                    arr[count].vals = NULL;
                    arr[count].val_count = 0;
                    found = count;
                    count++;
                }
                int vc = arr[found].val_count;
                arr[found].vals = realloc(arr[found].vals, (vc + 1) * sizeof(ValueNode));
                arr[found].vals[vc].value = strdup(val);
                arr[found].val_count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p = line + 7;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val = p + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        for (int j = 0; j < arr[i].val_count; j++) {
                            if (strcmp(arr[i].vals[j].value, val) == 0) {
                                free(arr[i].vals[j].value);
                                for (int k = j; k < arr[i].val_count - 1; k++) {
                                    arr[i].vals[k] = arr[i].vals[k + 1];
                                }
                                arr[i].val_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *key = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].key);
                    for (int j = 0; j < arr[i].val_count; j++) {
                        free(arr[i].vals[j].value);
                    }
                    free(arr[i].vals);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *key = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    for (int j = 0; j < arr[i].val_count; j++) {
                        printf("%s\n", arr[i].vals[j].value);
                    }
                    break;
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].key);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].key);
        for (int j = 0; j < arr[i].val_count; j++) {
            free(arr[i].vals[j].value);
        }
        free(arr[i].vals);
    }
    free(arr);
    return 0;
}
