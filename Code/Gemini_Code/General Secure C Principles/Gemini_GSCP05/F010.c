// F010.c
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
    char *key;
    char **values;
    int value_count;
    int value_capacity;
} KeyIndex;

int main(void) {
    KeyIndex *index = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val_start = p + 1;
                while (*val_start == ' ') val_start++;
                int k_idx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(index[i].key, key_start) == 0) {
                        k_idx = i;
                        break;
                    }
                }
                if (k_idx == -1) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        KeyIndex *new_idx = realloc(index, capacity * sizeof(KeyIndex));
                        if (new_idx) index = new_idx;
                    }
                    if (count < capacity) {
                        index[count].key = strdup(key_start);
                        index[count].values = NULL;
                        index[count].value_count = 0;
                        index[count].value_capacity = 0;
                        k_idx = count;
                        count++;
                    }
                }
                if (k_idx != -1) {
                    KeyIndex *ki = &index[k_idx];
                    if (ki->value_count >= ki->value_capacity) {
                        ki->value_capacity = ki->value_capacity == 0 ? 4 : ki->value_capacity * 2;
                        char **new_vals = realloc(ki->values, ki->value_capacity * sizeof(char *));
                        if (new_vals) ki->values = new_vals;
                    }
                    if (ki->value_count < ki->value_capacity) {
                        ki->values[ki->value_count] = strdup(val_start);
                        ki->value_count++;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *key_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val_start = p + 1;
                while (*val_start == ' ') val_start++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(index[i].key, key_start) == 0) {
                        for (int j = 0; j < index[i].value_count; j++) {
                            if (strcmp(index[i].values[j], val_start) == 0) {
                                free(index[i].values[j]);
                                for (int k = j; k < index[i].value_count - 1; k++) {
                                    index[i].values[k] = index[i].values[k + 1];
                                }
                                index[i].value_count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *key_start = line + 6;
            while (*key_start == ' ') key_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, key_start) == 0) {
                    free(index[i].key);
                    for (int j = 0; j < index[i].value_count; j++) {
                        free(index[i].values[j]);
                    }
                    free(index[i].values);
                    for (int j = i; j < count - 1; j++) {
                        index[j] = index[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *key_start = line + 7;
            while (*key_start == ' ') key_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(index[i].key, key_start) == 0) {
                    for (int j = 0; j < index[i].value_count; j++) {
                        printf("%s", index[i].values[j]);
                        if (j < index[i].value_count - 1) putchar(' ');
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", index[i].key);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(index[i].key);
        for (int j = 0; j < index[i].value_count; j++) {
            free(index[i].values[j]);
        }
        free(index[i].values);
    }
    free(index);
    return 0;
}
