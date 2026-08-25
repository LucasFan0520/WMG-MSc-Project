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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

int main(void) {
    Rule *rules = NULL;
    int count = 0;
    int cap = 0;
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
            char *old = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *nw = p + 1;
                while (*nw == ' ') nw++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    rules = realloc(rules, cap * sizeof(Rule));
                }
                rules[count].old_str = strdup(old);
                rules[count].new_str = strdup(nw);
                count++;
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
                    size_t new_len = strlen(rules[i].new_str);
                    size_t suffix_len = strlen(pos + old_len);
                    char *res = malloc(prefix_len + new_len + suffix_len + 1);
                    strncpy(res, text, prefix_len);
                    res[prefix_len] = '\0';
                    strcat(res, rules[i].new_str);
                    strcat(res, pos + old_len);
                    printf("%s\n", res);
                    free(res);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old = line + 7;
            while (*old == ' ') old++;
            int i = 0;
            while (i < count) {
                if (strcmp(rules[i].old_str, old) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (int j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            char *p = line + 5;
            char *endptr;
            long idx1 = strtol(p, &endptr, 10);
            long idx2 = strtol(endptr, NULL, 10);
            if (idx1 >= 0 && idx1 < count && idx2 >= 0 && idx2 < count) {
                Rule tmp = rules[idx1];
                rules[idx1] = rules[idx2];
                rules[idx2] = tmp;
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s -> %s\n", rules[i].old_str, rules[i].new_str);
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
