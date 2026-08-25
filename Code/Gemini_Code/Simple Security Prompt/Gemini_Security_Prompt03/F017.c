// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} ConfigPair;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    ConfigPair *list = NULL;
    int count = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        while (1) {
            char *line = read_line(fp);
            if (!line) break;
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                list = realloc(list, (count + 1) * sizeof(ConfigPair));
                list[count].key = strdup(line);
                list[count].value = strdup(eq + 1);
                count++;
            }
            free(line);
        }
        fclose(fp);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *val = p;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (list[i].key && strcmp(list[i].key, key) == 0) {
                    free(list[i].value);
                    list[i].value = strdup(val);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                list = realloc(list, (count + 1) * sizeof(ConfigPair));
                list[count].key = strdup(key);
                list[count].value = strdup(val);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].key && strcmp(list[i].key, key) == 0) {
                    free(list[i].key);
                    free(list[i].value);
                    list[i].key = NULL;
                    list[i].value = NULL;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (list[i].key && strcmp(list[i].key, key) == 0) {
                    printf("%s\n", list[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                if (list[i].key) {
                    printf("%s=%s\n", list[i].key, list[i].value);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (list[i].key) free(list[i].key);
        if (list[i].value) free(list[i].value);
    }
    free(list);
    return 0;
}
