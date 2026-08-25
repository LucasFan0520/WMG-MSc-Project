// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
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
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *old_val;
    char *new_val;
} Rule;

int main(void) {
    Rule *rules = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *old_val = p;
            while (*p && *p != ' ') p++;
            char *new_val = "";
            if (*p == ' ') {
                *p = '\0';
                new_val = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Rule *nb = realloc(rules, cap * sizeof(Rule));
                if (!nb) {
                    free(line);
                    break;
                }
                rules = nb;
            }
            rules[count].old_val = safe_dup(old_val);
            rules[count].new_val = safe_dup(new_val);
            count++;
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
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
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old_val = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(rules[i].old_val, old_val) == 0) {
                    free(rules[i].old_val);
                    free(rules[i].new_val);
                    for (size_t j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            char *p = line + 5;
            char *i_str = p;
            while (*p && *p != ' ') p++;
            char *j_str = "";
            if (*p == ' ') {
                *p = '\0';
                j_str = p + 1;
            }
            long long idx_i = strtoll(i_str, NULL, 10);
            long long idx_j = strtoll(j_str, NULL, 10);
            if (idx_i >= 0 && (size_t)idx_i < count && idx_j >= 0 && (size_t)idx_j < count) {
                Rule tmp = rules[idx_i];
                rules[idx_i] = rules[idx_j];
                rules[idx_j] = tmp;
            }
        } else if (strcmp(line, "RULES") == 0) {
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
