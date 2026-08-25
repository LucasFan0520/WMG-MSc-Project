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
    ConfigPair *config = NULL;
    int count = 0;
    int capacity = 0;

    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline = NULL;
            size_t flen = 0;
            ssize_t fread;
            while ((fread = getline(&fline, &flen, f)) != -1) {
                if (fread > 0 && fline[fread - 1] == '\n') fline[fread - 1] = '\0';
                char *eq = strchr(fline, '=');
                if (!eq) continue;
                *eq = '\0';
                char *key = fline;
                char *value = eq + 1;
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    ConfigPair *tmp = realloc(config, new_cap * sizeof(ConfigPair));
                    if (!tmp) return 1;
                    config = tmp;
                    capacity = new_cap;
                }
                config[count].key = strdup(key);
                config[count].value = strdup(value);
                count++;
            }
            free(fline);
            fclose(f);
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            char *value = "";
            if (space) {
                *space = '\0';
                value = space + 1;
                while (*value == ' ') value++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(config[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(config[found].value);
                config[found].value = strdup(value);
            } else {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    ConfigPair *tmp = realloc(config, new_cap * sizeof(ConfigPair));
                    if (!tmp) return 1;
                    config = tmp;
                    capacity = new_cap;
                }
                config[count].key = strdup(key);
                config[count].value = strdup(value);
                count++;
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            while (*key == ' ') key++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(config[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(config[found].key);
                free(config[found].value);
                for (int i = found; i < count - 1; i++) {
                    config[i] = config[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "GET ", 4) == 0) {
            char *key = line + 4;
            while (*key == ' ') key++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(config[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%s\n", config[found].value);
            else printf("NOT_FOUND\n");
        } else if (strcmp(line, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", config[i].key, config[i].value);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(config[i].key);
        free(config[i].value);
    }
    free(config);
    return 0;
}
