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
    if (argc < 2) {
        return 1;
    }
    ConfigPair *list = NULL;
    size_t count = 0;
    FILE *fp = fopen(argv[1], "r");
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    if (fp) {
        while ((read = getline(&line, &line_len, fp)) != -1) {
            if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
            if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
            char *eq = strchr(line, '=');
            if (!eq) continue;
            *eq = '\0';
            ConfigPair *temp = realloc(list, (count + 1) * sizeof(ConfigPair));
            if (temp) {
                list = temp;
                list[count].key = strdup(line);
                list[count].value = strdup(eq + 1);
                count++;
            }
        }
        fclose(fp);
    }
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') line[read - 1] = '\0';
        if (read > 1 && line[read - 2] == '\r') line[read - 2] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *val = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].value);
                    list[i].value = strdup(val);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                ConfigPair *temp = realloc(list, (count + 1) * sizeof(ConfigPair));
                if (temp) {
                    list = temp;
                    list[count].key = strdup(key);
                    list[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    free(list[i].key);
                    free(list[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, key) == 0) {
                    printf("%s\n", list[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", list[i].key, list[i].value);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    free(line);
    return 0;
}
