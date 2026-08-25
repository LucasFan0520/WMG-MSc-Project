// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Config;

int main(int argc, char **argv) {
    Config *cfg = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char fline[4096];
            while (fgets(fline, sizeof(fline), f)) {
                fline[strcspn(fline, "\r\n")] = 0;
                char *eq = strchr(fline, '=');
                if (eq) {
                    *eq = '\0';
                    cfg = realloc(cfg, (count + 1) * sizeof(Config));
                    cfg[count].key = strdup(fline);
                    cfg[count].value = strdup(eq + 1);
                    count++;
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, "");
            if (key && val) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfg[i].key, key) == 0) {
                        free(cfg[i].value);
                        cfg[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    cfg = realloc(cfg, (count + 1) * sizeof(Config));
                    cfg[count].key = strdup(key);
                    cfg[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfg[i].key, key) == 0) {
                        free(cfg[i].key);
                        free(cfg[i].value);
                        for (int j = i; j < count - 1; j++) {
                            cfg[j] = cfg[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(cfg);
                    cfg = NULL;
                } else {
                    cfg = realloc(cfg, count * sizeof(Config));
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfg[i].key, key) == 0) {
                        printf("%s\n", cfg[i].value);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", cfg[i].key, cfg[i].value);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(cfg[i].key);
        free(cfg[i].value);
    }
    free(cfg);
    return 0;
}
