// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char *value;
} Config;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    Config *configs = NULL;
    int count = 0;
    int capacity = 0;
    if (fp) {
        while (1) {
            char *line = read_line(fp);
            if (!line) break;
            char *eq = strchr(line, '=');
            if (eq) {
                char *k = malloc(eq - line + 1);
                memcpy(k, line, eq - line);
                k[eq - line] = '\0';
                char *v = strdup(eq + 1);
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    configs = realloc(configs, capacity * sizeof(Config));
                }
                configs[count].key = k;
                configs[count].value = v;
                count++;
            }
            free(line);
        }
        fclose(fp);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", configs[i].key, configs[i].value);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "SET") == 0) {
            char *key_start = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            int key_len = p - key_start;
            char *key = malloc(key_len + 1);
            memcpy(key, key_start, key_len);
            key[key_len] = '\0';
            while (*p == ' ' || *p == '\t') p++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(configs[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(configs[found].value);
                configs[found].value = strdup(p);
                free(key);
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    configs = realloc(configs, capacity * sizeof(Config));
                }
                configs[count].key = key;
                configs[count].value = strdup(p);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(p, "%255s", key) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(configs[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(configs[found].key);
                    free(configs[found].value);
                    for (int i = found; i < count - 1; i++) {
                        configs[i] = configs[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(p, "%255s", key) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(configs[i].key, key) == 0) {
                        printf("%s\n", configs[i].value);
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(configs[i].key);
        free(configs[i].value);
    }
    free(configs);
    return 0;
}
