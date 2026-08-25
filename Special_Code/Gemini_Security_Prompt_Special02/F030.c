// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *old_pattern;
    char *new_pattern;
} RenameRule;

int main() {
    RenameRule *rules = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *arg1 = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *new_pat = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                rules = realloc(rules, cap * sizeof(RenameRule));
            }
            rules[count].old_pattern = mystrdup(arg1);
            rules[count].new_pattern = mystrdup(new_pat);
            count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            if (arg1 && *arg1) {
                if (p && *p) {
                    *(p - 1) = ' ';
                }
            }
            int matched = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(rules[i].old_pattern, arg1) == 0) {
                    printf("%s\n", rules[i].new_pattern);
                    matched = 1;
                    break;
                }
            }
            if (!matched) {
                printf("%s\n", arg1);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(rules[i].old_pattern, arg1) == 0) {
                    free(rules[i].old_pattern);
                    free(rules[i].new_pattern);
                    for (size_t j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            while (*p == ' ') p++;
            char *j_s = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idx1 = atoi(arg1);
            int idx2 = atoi(j_s);
            if (idx1 >= 0 && (size_t)idx1 < count && idx2 >= 0 && (size_t)idx2 < count) {
                RenameRule tmp = rules[idx1];
                rules[idx1] = rules[idx2];
                rules[idx2] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s -> %s\n", rules[i].old_pattern, rules[i].new_pattern);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(rules[i].old_pattern);
        free(rules[i].new_pattern);
    }
    free(rules);
    return 0;
}
