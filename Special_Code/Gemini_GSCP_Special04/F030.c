/* F030.c */
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
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *old_r;
    char *new_r;
} Rule;

int main(void) {
    Rule *rules = NULL;
    size_t count = 0;
    size_t cap = 0;
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
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *old_r = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *new_r = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Rule *nr = realloc(rules, cap * sizeof(Rule));
                if (nr) rules = nr;
            }
            char *no = mystrdup(old_r);
            char *nn = mystrdup(new_r);
            if (no && nn) {
                rules[count].old_r = no;
                rules[count].new_r = nn;
                count++;
            } else {
                free(no); free(nn);
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int applied = 0;
            for (size_t i = 0; i < count; i++) {
                char *found = strstr(text, rules[i].old_r);
                if (found) {
                    size_t prefix_len = found - text;
                    size_t old_len = strlen(rules[i].old_r);
                    size_t new_len = strlen(rules[i].new_r);
                    size_t suffix_len = strlen(found + old_len);
                    char *res = malloc(prefix_len + new_len + suffix_len + 1);
                    if (res) {
                        strncpy(res, text, prefix_len);
                        strcpy(res + prefix_len, rules[i].new_r);
                        strcpy(res + prefix_len + new_len, found + old_len);
                        printf("%s\n", res);
                        free(res);
                    }
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *old_r = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(rules[i].old_r, old_r) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(rules[found].old_r);
                free(rules[found].new_r);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    rules[i] = rules[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            while (*p == ' ') p++;
            char *i_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *j_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long idxI = strtoll(i_str, NULL, 10);
            long long idxJ = strtoll(j_str, NULL, 10);
            if (idxI >= 0 && idxI < (long long)count && idxJ >= 0 && idxJ < (long long)count) {
                Rule temp = rules[idxI];
                rules[idxI] = rules[idxJ];
                rules[idxJ] = temp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s -> %s\n", rules[i].old_r, rules[i].new_r);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(rules[i].old_r);
        free(rules[i].new_r);
    }
    free(rules);
    return 0;
}
