// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

typedef struct {
    char *key;
    char *value;
} Variable;

int main() {
    Variable *vars = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
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
                    if (strcmp(vars[i].key, key) == 0) {
                        free(vars[i].value);
                        vars[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    vars = realloc(vars, (count + 1) * sizeof(Variable));
                    vars[count].key = strdup(key);
                    vars[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    free(vars[i].key);
                    free(vars[i].value);
                    for (int j = i; j < count - 1; j++) {
                        vars[j] = vars[j + 1];
                    }
                    count--;
                    vars = realloc(vars, count * sizeof(Variable));
                    break;
                }
            }
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            char *text = line + 7;
            int i = 0;
            while (text[i] != '\0') {
                if (text[i] == '{') {
                    int j = i + 1;
                    while (text[j] != '\0' && text[j] != '}') {
                        j++;
                    }
                    if (text[j] == '}') {
                        int len = j - i - 1;
                        char *k = malloc(len + 1);
                        memcpy(k, text + i + 1, len);
                        k[len] = '\0';
                        int found = 0;
                        for (int v = 0; v < count; v++) {
                            if (strcmp(vars[v].key, k) == 0) {
                                printf("%s", vars[v].value);
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            for (int v = i; v <= j; v++) {
                                putchar(text[v]);
                            }
                        }
                        free(k);
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
            printf("\n");
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
