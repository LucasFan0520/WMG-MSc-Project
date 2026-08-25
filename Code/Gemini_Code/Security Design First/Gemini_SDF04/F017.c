// F017.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} ConfigPair;

ConfigPair *pairs = NULL;
int count = 0;
int capacity = 0;

int find_key(const char *key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(pairs[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void set_pair(const char *key, const char *value) {
    int idx = find_key(key);
    if (idx != -1) {
        char *nv = strdup(value);
        if (nv) {
            free(pairs[idx].value);
            pairs[idx].value = nv;
        }
    } else {
        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            ConfigPair *tmp = realloc(pairs, capacity * sizeof(ConfigPair));
            if (!tmp) return;
            pairs = tmp;
        }
        pairs[count].key = strdup(key);
        pairs[count].value = strdup(value);
        if (pairs[count].key && pairs[count].value) {
            count++;
        }
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        return 1;
    }
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t nread;
        while ((nread = getline(&fline, &flen, f)) != -1) {
            size_t l = strlen(fline);
            if (l > 0 && fline[l - 1] == '\n') fline[l - 1] = '\0';
            char *eq = strchr(fline, '=');
            if (eq) {
                *eq = '\0';
                set_pair(fline, eq + 1);
            }
        }
        free(fline);
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') line[l - 1] = '\0';
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "SET") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    set_pair(arg1, space2 + 1);
                }
            } else if (strcmp(cmd, "UNSET") == 0) {
                int idx = find_key(arg1);
                if (idx != -1) {
                    free(pairs[idx].key);
                    free(pairs[idx].value);
                    for (int i = idx; i < count - 1; i++) {
                        pairs[i] = pairs[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "GET") == 0) {
                int idx = find_key(arg1);
                if (idx != -1) {
                    printf("%s\n", pairs[idx].value);
                }
            }
        } else {
            if (strcmp(cmd, "WRITE") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s=%s\n", pairs[i].key, pairs[i].value);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(pairs[i].key);
        free(pairs[i].value);
    }
    free(pairs);
    return 0;
}