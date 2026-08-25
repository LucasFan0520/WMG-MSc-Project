// F004.c
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
    char *value;
} Variable;

int main(void) {
    Variable *vars = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *val_start = p + 1;
                while (*val_start == ' ') val_start++;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key_start) == 0) {
                        free(vars[i].value);
                        vars[i].value = strdup(val_start);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Variable *new_v = realloc(vars, capacity * sizeof(Variable));
                        if (new_v) vars = new_v;
                    }
                    if (count < capacity) {
                        vars[count].key = strdup(key_start);
                        vars[count].value = strdup(val_start);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key_start = line + 6;
            while (*key_start == ' ') key_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key_start) == 0) {
                    free(vars[i].key);
                    free(vars[i].value);
                    for (int j = i; j < count - 1; j++) {
                        vars[j] = vars[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            char *text = line + 7;
            size_t i = 0;
            while (text[i] != '\0') {
                if (text[i] == '{') {
                    size_t j = i + 1;
                    while (text[j] != '\0' && text[j] != '}') {
                        j++;
                    }
                    if (text[j] == '}') {
                        size_t key_len = j - (i + 1);
                        char *k_buf = malloc(key_len + 1);
                        if (k_buf) {
                            memcpy(k_buf, text + i + 1, key_len);
                            k_buf[key_len] = '\0';
                            int found = 0;
                            for (int k = 0; k < count; k++) {
                                if (strcmp(vars[k].key, k_buf) == 0) {
                                    printf("%s", vars[k].value);
                                    found = 1;
                                    break;
                                }
                            }
                            if (!found) {
                                for (size_t m = i; m <= j; m++) {
                                    putchar(text[m]);
                                }
                            }
                            free(k_buf);
                        }
                        i = j + 1;
                    } else {
                        putchar(text[i]);
                        i++;
                    }
                } else {
                    putchar(text[i]);
                    i++;
                }
            }
            putchar('\n');
        } else if (strcmp(line, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
