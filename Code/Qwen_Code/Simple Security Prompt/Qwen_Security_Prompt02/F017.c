// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} Config;

Config *configs = NULL;
int count = 0;

int find_key(const char *key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(configs[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[10000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = 0;
            configs = realloc(configs, sizeof(Config) * (count + 1));
            configs[count].key = strdup(line);
            configs[count].val = strdup(eq + 1);
            count++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char cmd[20];
    char key[1000];
    char val[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key);
            fgets(val, sizeof(val), stdin);
            if (val[0] == ' ') {
                memmove(val, val + 1, strlen(val));
            }
            val[strcspn(val, "\n")] = 0;
            int idx = find_key(key);
            if (idx != -1) {
                free(configs[idx].val);
                configs[idx].val = strdup(val);
            } else {
                configs = realloc(configs, sizeof(Config) * (count + 1));
                configs[count].key = strdup(key);
                configs[count].val = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                free(configs[idx].key);
                free(configs[idx].val);
                for (int i = idx; i < count - 1; i++) {
                    configs[i] = configs[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                printf("%s\n", configs[idx].val);
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", configs[i].key, configs[i].val);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(configs[i].key);
        free(configs[i].val);
    }
    free(configs);

    return 0;
}