/* F017.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} KV;

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    KV *config = NULL;
    size_t count = 0;
    size_t capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t len = 0;

    if (fp) {
        while (getline(&line, &len, fp) != -1) {
            size_t str_len = strlen(line);
            while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
                line[str_len - 1] = '\0';
                str_len--;
            }
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *key = line;
                char *value = eq + 1;

                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    KV *temp = realloc(config, new_cap * sizeof(KV));
                    if (!temp) break;
                    config = temp;
                    capacity = new_cap;
                }
                config[count].key = strdup(key);
                config[count].value = strdup(value);
                if (config[count].key && config[count].value) {
                    count++;
                }
            }
        }
        fclose(fp);
    }

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;

        if (strcmp(cmd, "SET") == 0) {
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *value = p;

            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(config[i].key, key) == 0) {
                    char *temp = strdup(value);
                    if (temp) {
                        free(config[i].value);
                        config[i].value = temp;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    KV *temp = realloc(config, new_cap * sizeof(KV));
                    if (!temp) break;
                    config = temp;
                    capacity = new_cap;
                }
                config[count].key = strdup(key);
                config[count].value = strdup(value);
                if (config[count].key && config[count].value) {
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(config[i].key, key) == 0) {
                    free(config[i].key);
                    free(config[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        config[j] = config[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(config[i].key, key) == 0) {
                    printf("%s\n", config[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", config[i].key, config[i].value);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(config[i].key);
        free(config[i].value);
    }
    free(config);
    return 0;
}
