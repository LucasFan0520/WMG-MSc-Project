// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *key;
    char *val;
} Config;

int main(int argc, char **argv) {
    Config *cfgs = NULL;
    int count = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            while (1) {
                char *line = read_line(f);
                if (!line) break;
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    cfgs = realloc(cfgs, sizeof(Config) * (count + 1));
                    cfgs[count].key = strdup(line);
                    cfgs[count].val = strdup(eq + 1);
                    count++;
                }
                free(line);
            }
            fclose(f);
        }
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "WRITE") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s=%s\n", cfgs[i].key, cfgs[i].val);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "SET") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *key = args;
                char *val = p2 + 1;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(cfgs[i].key, key) == 0) {
                        free(cfgs[i].val);
                        cfgs[i].val = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    cfgs = realloc(cfgs, sizeof(Config) * (count + 1));
                    cfgs[count].key = strdup(key);
                    cfgs[count].val = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(cfgs[i].key, key) == 0) {
                    free(cfgs[i].key);
                    free(cfgs[i].val);
                    for (int j = i; j < count - 1; j++) {
                        cfgs[j] = cfgs[j + 1];
                    }
                    count--;
                    if (count == 0) { free(cfgs); cfgs = NULL; }
                    else { cfgs = realloc(cfgs, sizeof(Config) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(cfgs[i].key, key) == 0) {
                    printf("%s\n", cfgs[i].val);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(cfgs[i].key);
        free(cfgs[i].val);
    }
    free(cfgs);
    return 0;
}
