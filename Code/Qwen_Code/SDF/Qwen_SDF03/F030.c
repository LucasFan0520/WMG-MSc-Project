// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct {
    char *old;
    char *new;
} Rule;

Rule *rules = NULL;
size_t count = 0, cap = 0;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *old = get_token(&ptr);
            if (old) {
                if (count == cap) {
                    size_t new_cap = cap ? cap * 2 : 4;
                    Rule *tmp = realloc(rules, new_cap * sizeof(Rule));
                    if (!tmp) continue;
                    rules = tmp;
                    cap = new_cap;
                }
                rules[count].old = strdup(old);
                rules[count].new = strdup(ptr);
                count++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = ptr;
            int matched = 0;
            for (size_t i = 0; i < count; i++) {
                char *pos = strstr(text, rules[i].old);
                if (pos) {
                    size_t pre = pos - text;
                    size_t old_len = strlen(rules[i].old);
                    size_t new_len = strlen(rules[i].new);
                    size_t total = pre + new_len + strlen(pos + old_len) + 1;
                    char *buf = malloc(total);
                    if (buf) {
                        memcpy(buf, text, pre);
                        memcpy(buf + pre, rules[i].new, new_len);
                        strcpy(buf + pre + new_len, pos + old_len);
                        printf("%s\n", buf);
                        free(buf);
                    }
                    matched = 1;
                    break;
                }
            }
            if (!matched) printf("%s\n", text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old = get_token(&ptr);
            if (old) {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(rules[i].old, old) == 0) {
                        free(rules[i].old);
                        free(rules[i].new);
                        for (size_t j = i; j < count - 1; j++) rules[j] = rules[j + 1];
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *istr = get_token(&ptr);
            char *jstr = get_token(&ptr);
            if (istr && jstr) {
                size_t i = strtoul(istr, NULL, 10);
                size_t j = strtoul(jstr, NULL, 10);
                if (i < count && j < count) {
                    Rule tmp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = tmp;
                }
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", rules[i].old, rules[i].new);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(rules[i].old);
        free(rules[i].new);
    }
    free(rules);
    return 0;
}