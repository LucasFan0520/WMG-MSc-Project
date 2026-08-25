// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *old_val;
    char *new_val;
} Rule;

int main() {
    Rule *rules = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d: %s -> %s\n", i, rules[i].old_val, rules[i].new_val);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char old[256], nw[256];
            if (sscanf(p, "%255s %255s", old, nw) == 2) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    rules = realloc(rules, capacity * sizeof(Rule));
                }
                rules[count].old_val = strdup(old);
                rules[count].new_val = strdup(nw);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char old[256];
            if (sscanf(p, "%255s", old) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(rules[i].old_val, old) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(rules[found].old_val);
                    free(rules[found].new_val);
                    for (int i = found; i < count - 1; i++) {
                        rules[i] = rules[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i = 0, j = 0;
            if (sscanf(p, "%d %d", &i, &j) == 2) {
                if (i >= 0 && i < count && j >= 0 && j < count) {
                    Rule temp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = temp;
                }
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char text[256];
            if (sscanf(p, "%255s", text) == 1) {
                int matched = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(rules[i].old_val, text) == 0) {
                        matched = i;
                        break;
                    }
                }
                if (matched != -1) {
                    printf("%s\n", rules[matched].new_val);
                } else {
                    printf("%s\n", text);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(rules[i].old_val);
        free(rules[i].new_val);
    }
    free(rules);
    return 0;
}
