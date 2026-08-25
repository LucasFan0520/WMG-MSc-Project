// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} ConfigPair;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(int argc, char **argv) {
    ConfigPair *list = NULL;
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
                    list = realloc(list, (count + 1) * sizeof(ConfigPair));
                    list[count].key = strdup(key);
                    list[count].value = strdup(value);
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
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "WRITE") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s=%s\n", list[i].key, list[i].value);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "SET") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *key = rest;
                char *value = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(list[found].value);
                    list[found].value = strdup(value);
                } else {
                    list = realloc(list, (count + 1) * sizeof(ConfigPair));
                    list[count].key = strdup(key);
                    list[count].value = strdup(value);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].key);
                free(list[found].value);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(ConfigPair));
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = rest;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    printf("%s\n", list[i].value);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    return 0;
}