// F017.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} ConfigPair;

int main(int argc, char **argv) {
    ConfigPair *cfg = NULL;
    int count = 0;

    if (argc >= 2) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread;
            while ((fread = getline(&fline, &flen, fp)) != -1) {
                fline[strcspn(fline, "\r\n")] = '\0';
                char *eq = strchr(fline, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = fline;
                    char *val = eq + 1;
                    cfg = realloc(cfg, (count + 1) * sizeof(ConfigPair));
                    if (!cfg) return 1;
                    cfg[count].key = strdup(key);
                    cfg[count].value = strdup(val);
                    count++;
                }
            }
            free(fline);
            fclose(fp);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "SET") == 0 && args) {
            char *key = args;
            char *space2 = strchr(args, ' ');
            char *value = "";
            if (space2) {
                *space2 = '\0';
                value = space2 + 1;
                while (*value == ' ') value++;
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, key) == 0) {
                    free(cfg[i].value);
                    cfg[i].value = strdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                cfg = realloc(cfg, (count + 1) * sizeof(ConfigPair));
                if (!cfg) return 1;
                cfg[count].key = strdup(key);
                cfg[count].value = strdup(value);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, args) == 0) {
                    free(cfg[i].key);
                    free(cfg[i].value);
                    for (int j = i; j < count - 1; j++) {
                        cfg[j] = cfg[j + 1];
                    }
                    count--;
                    cfg = realloc(cfg, count * sizeof(ConfigPair));
                    if (count > 0 && !cfg) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(cfg[i].key, args) == 0) {
                    printf("%s\n", cfg[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", cfg[i].key, cfg[i].value);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(cfg[i].key);
        free(cfg[i].value);
    }
    free(cfg);
    return 0;
}
