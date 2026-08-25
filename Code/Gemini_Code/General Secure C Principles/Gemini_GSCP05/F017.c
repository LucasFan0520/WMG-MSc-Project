// F017.c
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
} Config;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (!fp) return 1;
    Config *cfg = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(fp);
        if (!line) break;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char *k = line;
            char *v = eq + 1;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Config *new_cfg = realloc(cfg, capacity * sizeof(Config));
                if (new_cfg) cfg = new_cfg;
            }
            if (count < capacity) {
                cfg[count].key = strdup(k);
                cfg[count].value = strdup(v);
                count++;
            }
        }
        free(line);
    }
    fclose(fp);
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
                    if (strcmp(cfg[i].key, key_start) == 0) {
                        free(cfg[i].value);
                        cfg[i].value = strdup(val_start);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Config *new_cfg = realloc(cfg, capacity * sizeof(Config));
                        if (new_cfg) cfg = new_cfg;
                    }
                    if (count < capacity) {
                        cfg[count].key = strdup(key_start);
                        cfg[count].value = strdup(val_start);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key_start = line + 6;
            while (*key_start == ' ') key_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, key_start) == 0) {
                    free(cfg[i].key);
                    free(cfg[i].value);
                    for (int j = i; j < count - 1; j++) {
                        cfg[j] = cfg[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key_start = line + 4;
            while (*key_start == ' ') key_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, key_start) == 0) {
                    printf("%s\n", cfg[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", cfg[i].key, cfg[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(cfg[i].key);
        free(cfg[i].value);
    }
    free(cfg);
    return 0;
}
