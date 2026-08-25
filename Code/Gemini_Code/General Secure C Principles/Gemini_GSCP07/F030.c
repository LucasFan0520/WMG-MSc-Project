// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_val;
    char *new_val;
} Rule;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Rule *rules = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *old_val = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *new_val = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Rule *nr = realloc(rules, capacity * sizeof(Rule));
                if (!nr) break;
                rules = nr;
            }
            rules[count].old_val = strdup(old_val);
            rules[count].new_val = strdup(new_val);
            count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(text, rules[i].old_val) == 0) {
                    printf("%s\n", rules[i].new_val);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *old_val = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t i = 0;
            while (i < count) {
                if (strcmp(rules[i].old_val, old_val) == 0) {
                    free(rules[i].old_val);
                    free(rules[i].new_val);
                    for (size_t j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            while (*p == ' ') p++;
            char *i_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *j_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idx_i = atoi(i_str);
            int idx_j = atoi(j_str);
            if (idx_i >= 0 && idx_i < (int)count && idx_j >= 0 && idx_j < (int)count) {
                Rule tmp = rules[idx_i];
                rules[idx_i] = rules[idx_j];
                rules[idx_j] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s -> %s\n", rules[i].old_val, rules[i].new_val);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(rules[i].old_val);
        free(rules[i].new_val);
    }
    free(rules);
    return 0;
}
