// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

int main(void) {
    Rule *rules = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *old_s = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *new_s = p + 1;
                while (*new_s == ' ') new_s++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Rule *new_r = realloc(rules, capacity * sizeof(Rule));
                    if (new_r) rules = new_r;
                }
                if (count < capacity) {
                    rules[count].old_str = strdup(old_s);
                    rules[count].new_str = strdup(new_s);
                    count++;
                }
            }
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            while (*text == ' ') text++;
            int applied = 0;
            for (int i = 0; i < count; i++) {
                char *pos = strstr(text, rules[i].old_str);
                if (pos) {
                    size_t prefix_len = pos - text;
                    size_t old_len = strlen(rules[i].old_str);
                    printf("%.*s%s%s\n", (int)prefix_len, text, rules[i].new_str, pos + old_len);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old_s = line + 7;
            while (*old_s == ' ') old_s++;
            for (int i = 0; i < count; i++) {
                if (strcmp(rules[i].old_str, old_s) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (int j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *endptr;
            long i = strtol(p, &endptr, 10);
            if (endptr != p) {
                p = endptr;
                while (*p == ' ') p++;
                long j = strtol(p, &endptr, 10);
                if (endptr != p && i >= 0 && i < count && j >= 0 && j < count) {
                    Rule temp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = temp;
                }
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d: %s -> %s\n", i, rules[i].old_str, rules[i].new_str);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    return 0;
}
