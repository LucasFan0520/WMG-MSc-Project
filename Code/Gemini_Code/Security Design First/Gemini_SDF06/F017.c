// F017.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Config;

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }

    Config *cfg = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0';
        char *key = line;
        char *value = eq + 1;

        if (count >= capacity) {
            size_t new_cap = capacity == 0 ? 4 : capacity * 2;
            Config *new_cfg = realloc(cfg, new_cap * sizeof(Config));
            if (!new_cfg) continue;
            cfg = new_cfg;
            capacity = new_cap;
        }

        char *k = strdup(key);
        char *v = strdup(value);
        if (k && v) {
            cfg[count].key = k;
            cfg[count].value = v;
            count++;
        } else {
            free(k);
            free(v);
        }
    }
    fclose(fp);

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *value = p;

            if (*key == '\0') continue;

            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, key) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }

            if (found_idx != -1) {
                char *v = strdup(value);
                if (v) {
                    free(cfg[found_idx].value);
                    cfg[found_idx].value = v;
                }
            } else {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Config *new_cfg = realloc(cfg, new_cap * sizeof(Config));
                    if (!new_cfg) continue;
                    cfg = new_cfg;
                    capacity = new_cap;
                }
                char *k = strdup(key);
                char *v = strdup(value);
                if (k && v) {
                    cfg[count].key = k;
                    cfg[count].value = v;
                    count++;
                } else {
                    free(k);
                    free(v);
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*key == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, key) == 0) {
                    free(cfg[i].key);
                    free(cfg[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        cfg[j] = cfg[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*key == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, key) == 0) {
                    printf("%s\n", cfg[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", cfg[i].key, cfg[i].value);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(cfg[i].key);
        free(cfg[i].value);
    }
    free(cfg);
    free(line);
    return 0;
}
