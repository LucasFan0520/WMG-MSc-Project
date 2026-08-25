// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *key;
    char *value;
} Pair;

int main(void) {
    Pair *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "REMOVE") == 0) {
            char *key = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *value = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (strcmp(cmd, "ADD") == 0) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Pair *nb = realloc(list, cap * sizeof(Pair));
                    if (nb) list = nb;
                }
                if (count < cap) {
                    list[count].key = strdup(key);
                    list[count].value = strdup(value);
                    count++;
                }
            } else {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(list[i].key, key) == 0 && strcmp(list[i].value, value) == 0) {
                        free(list[i].key);
                        free(list[i].value);
                        for (size_t j = i; j < count - 1; j++) {
                            list[j] = list[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            size_t i = 0;
            while (i < count) {
                if (strcmp(list[i].key, arg1) == 0) {
                    free(list[i].key);
                    free(list[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].key, arg1) == 0) {
                    printf("%s\n", list[i].value);
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            char **seen = NULL;
            size_t s_count = 0;
            for (size_t i = 0; i < count; i++) {
                int found = 0;
                for (size_t j = 0; j < s_count; j++) {
                    if (strcmp(seen[j], list[i].key) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s\n", list[i].key);
                    seen = realloc(seen, (s_count + 1) * sizeof(char *));
                    if (seen) {
                        seen[s_count] = list[i].key;
                        s_count++;
                    }
                }
            }
            free(seen);
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].key);
        free(list[i].value);
    }
    free(list);
    return 0;
}
