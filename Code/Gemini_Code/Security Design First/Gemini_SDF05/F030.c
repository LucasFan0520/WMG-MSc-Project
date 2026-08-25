// F030.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

int main(void) {
    Rule *rules = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *old_str = p;
            char *space = strchr(old_str, ' ');
            char *new_str = "";
            if (space) {
                *space = '\0';
                new_str = space + 1;
                while (*new_str == ' ') new_str++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Rule *tmp = realloc(rules, new_cap * sizeof(Rule));
                if (!tmp) return 1;
                rules = tmp;
                capacity = new_cap;
            }
            rules[count].old_str = strdup(old_str);
            rules[count].new_str = strdup(new_str);
            count++;
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            while (*text == ' ') text++;
            int applied = 0;
            for (int i = 0; i < count; i++) {
                char *pos = strstr(text, rules[i].old_str);
                if (pos) {
                    int prefix_len = pos - text;
                    printf("%.*s%s%s\n", prefix_len, text, rules[i].new_str, pos + strlen(rules[i].old_str));
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old_str = line + 7;
            while (*old_str == ' ') old_str++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(rules[i].old_str, old_str) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(rules[found].old_str);
                free(rules[found].new_str);
                for (int i = found; i < count - 1; i++) {
                    rules[i] = rules[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *endptr1;
            long idx1 = strtol(p, &endptr1, 10);
            if (p == endptr1 || idx1 < 0 || idx1 >= count) continue;
            while (*endptr1 == ' ') endptr1++;
            char *endptr2;
            long idx2 = strtol(endptr1, &endptr2, 10);
            if (endptr1 == endptr2 || idx2 < 0 || idx2 >= count) continue;
            if (idx1 == idx2) continue;
            Rule tmp = rules[idx1];
            rules[idx1] = rules[idx2];
            rules[idx2] = tmp;
        } else if (strcmp(line, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s -> %s\n", rules[i].old_str, rules[i].new_str);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    return 0;
}
