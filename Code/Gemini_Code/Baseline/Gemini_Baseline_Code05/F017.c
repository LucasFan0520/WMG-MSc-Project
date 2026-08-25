// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Config;

Config *cfg = NULL;
int count = 0;
int capacity = 0;

int find_key(const char *key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(cfg[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void set_key(const char *key, const char *value) {
    int idx = find_key(key);
    if (idx != -1) {
        free(cfg[idx].value);
        cfg[idx].value = strdup(value);
    } else {
        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            cfg = realloc(cfg, capacity * sizeof(Config));
        }
        cfg[count].key = strdup(key);
        cfg[count].value = strdup(value);
        count++;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char fline[4096];
            while (fgets(fline, sizeof(fline), f)) {
                fline[strcspn(fline, "\r\n")] = 0;
                char *eq = strchr(fline, '=');
                if (eq) {
                    *eq = 0;
                    set_key(fline, eq + 1);
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p1 = line + 4;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                set_key(p1, p2 + 1);
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            int idx = find_key(key);
            if (idx != -1) {
                free(cfg[idx].key);
                free(cfg[idx].value);
                for (int i = idx; i < count - 1; i++) {
                    cfg[i] = cfg[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            int idx = find_key(key);
            if (idx != -1) {
                printf("%s\n", cfg[idx].value);
            }
        } else if (strcmp(line, "WRITE") == 0) {
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
