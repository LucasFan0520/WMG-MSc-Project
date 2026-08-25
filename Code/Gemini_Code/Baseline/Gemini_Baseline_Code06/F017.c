// F017.c
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
} ConfigPair;

int main(int argc, char *argv[]) {
    ConfigPair *config = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            while (1) {
                char *line = read_line(fp);
                if (!line) break;
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = line;
                    char *value = eq + 1;
                    config = realloc(config, (count + 1) * sizeof(ConfigPair));
                    config[count].key = strdup(key);
                    config[count].value = strdup(value);
                    count++;
                }
                free(line);
            }
            fclose(fp);
        }
    }
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
                char *value = p + 1;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(config[i].key, key) == 0) {
                        free(config[i].value);
                        config[i].value = strdup(value);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    config = realloc(config, (count + 1) * sizeof(ConfigPair));
                    config[count].key = strdup(key);
                    config[count].value = strdup(value);
                    count++;
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(config[i].key, key) == 0) {
                    free(config[i].key);
                    free(config[i].value);
                    for (int j = i; j < count - 1; j++) {
                        config[j] = config[j + 1];
                    }
                    count--;
                    config = realloc(config, count * sizeof(ConfigPair));
                    break;
                }
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            for (int i = 0; i < count; i++) {
                if (strcmp(config[i].key, key) == 0) {
                    printf("%s\n", config[i].value);
                    break;
                }
            }
        } else if (strcmp(line, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", config[i].key, config[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(config[i].key);
        free(config[i].value);
    }
    free(config);
    return 0;
}
