// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Config;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Config *cfgs = NULL;
    int count = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        char fline[1024];
        while (fgets(fline, sizeof(fline), fp)) {
            fline[strcspn(fline, "\r\n")] = 0;
            char *eq = strchr(fline, '=');
            if (eq) {
                *eq = '\0';
                char *k = fline;
                char *v = eq + 1;
                cfgs = realloc(cfgs, (count + 1) * sizeof(Config));
                cfgs[count].key = strdup(k);
                cfgs[count].value = strdup(v);
                count++;
            }
        }
        fclose(fp);
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            int key_bytes = 0;
            if (sscanf(args, "%255s%n", key, &key_bytes) == 1) {
                char *value = args + key_bytes;
                while (*value == ' ') value++;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfgs[i].key, key) == 0) {
                        free(cfgs[i].value);
                        cfgs[i].value = strdup(value);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    cfgs = realloc(cfgs, (count + 1) * sizeof(Config));
                    cfgs[count].key = strdup(key);
                    cfgs[count].value = strdup(value);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(args, "%255s", key) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfgs[i].key, key) == 0) {
                        free(cfgs[i].key);
                        free(cfgs[i].value);
                        for (int j = i; j < count - 1; j++) {
                            cfgs[j] = cfgs[j + 1];
                        }
                        count--;
                        cfgs = realloc(cfgs, count * sizeof(Config));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(args, "%255s", key) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfgs[i].key, key) == 0) {
                        printf("%s\n", cfgs[i].value);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", cfgs[i].key, cfgs[i].value);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(cfgs[i].key);
        free(cfgs[i].value);
    }
    free(cfgs);
    return 0;
}
