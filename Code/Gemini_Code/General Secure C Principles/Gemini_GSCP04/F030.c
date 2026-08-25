// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *old_val;
    char *new_val;
} RenameRule;

int main(void) {
    RenameRule *rules = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *old_val = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *new_val = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*old_val != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RenameRule *new_rules = realloc(rules, capacity * sizeof(RenameRule));
                    if (new_rules) rules = new_rules;
                }
                if (count < capacity) {
                    rules[count].old_val = strdup(old_val);
                    rules[count].new_val = strdup(new_val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                char *loc = strstr(text, rules[i].old_val);
                if (loc) {
                    *loc = '\0';
                    printf("%s%s%s\n", text, rules[i].new_val, loc + strlen(rules[i].old_val));
                    *loc = rules[i].old_val[0];
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *old_val = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            size_t idx = 0;
            while (idx < count) {
                if (strcmp(rules[idx].old_val, old_val) == 0) {
                    free(rules[idx].old_val);
                    free(rules[idx].new_val);
                    for (size_t j = idx; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                } else {
                    idx++;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *i_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *j_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long idx_i = atoll(i_s);
            long long idx_j = atoll(j_s);
            if (idx_i >= 0 && (size_t)idx_i < count && idx_j >= 0 && (size_t)idx_j < count && idx_i != idx_j) {
                RenameRule tmp = rules[idx_i];
                rules[idx_i] = rules[idx_j];
                rules[idx_j] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("[%zu] %s -> %s\n", i, rules[i].old_val, rules[i].new_val);
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
