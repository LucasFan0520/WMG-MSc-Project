// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} ConfigPair;

int main(int argc, char *argv[]) {
    ConfigPair *cfg = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char fline[1024];
            while (fgets(fline, sizeof(fline), f)) {
                int flen = strlen(fline);
                while (flen > 0 && (fline[flen - 1] == '\n' || fline[flen - 1] == '\r')) {
                    fline[flen - 1] = '\0';
                    flen--;
                }
                char *eq = strchr(fline, '=');
                if (eq) {
                    *eq = '\0';
                    cfg = realloc(cfg, (count + 1) * sizeof(ConfigPair));
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
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            int k_len = 0;
            if (sscanf(ptr, "%255s%n", key, &k_len) > 0) {
                char *val = ptr + k_len;
                while (*val == ' ') val++;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfg[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(cfg[found].value);
                    cfg[found].value = strdup(val);
                } else {
                    cfg = realloc(cfg, (count + 1) * sizeof(ConfigPair));
                    cfg[count].key = strdup(key);
                    cfg[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(ptr, "%255s", key) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfg[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(cfg[found].key);
                    free(cfg[found].value);
                    for (int i = found; i < count - 1; i++) {
                        cfg[i] = cfg[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        cfg = realloc(cfg, count * sizeof(ConfigPair));
                    } else {
                        free(cfg);
                        cfg = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char key[256];
            if (sscanf(ptr, "%255s", key) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfg[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s\n", cfg[found].value);
                } else {
                    printf("NOT_FOUND\n");
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
