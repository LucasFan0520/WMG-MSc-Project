// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} RenameRule;

int main(void) {
    RenameRule *rules = NULL;
    int count = 0;
    char *line = NULL;
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char o[256], n[256];
            if (sscanf(line + 4, "%255s %255s", o, n) == 2) {
                RenameRule *tmp = realloc(rules, (count + 1) * sizeof(RenameRule));
                if (tmp) {
                    rules = tmp;
                    rules[count].old_str = strdup(o);
                    rules[count].new_str = strdup(n);
                    count++;
                }
            }
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            while (*text == ' ') text++;
            char *res = NULL;
            for (int i = 0; i < count; i++) {
                if (strcmp(text, rules[i].old_str) == 0) {
                    res = rules[i].new_str;
                    break;
                }
            }
            if (res) {
                printf("%s\n", res);
            } else {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old_str = line + 7;
            while (*old_str == ' ') old_str++;
            int i = 0;
            while (i < count) {
                if (strcmp(rules[i].old_str, old_str) == 0) {
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
            int i, j;
            if (sscanf(line + 5, "%d %d", &i, &j) == 2) {
                if (i >= 0 && i < count && j >= 0 && j < count) {
                    RenameRule tmp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = tmp;
                }
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("[%d] %s -> %s\n", i, rules[i].old_str, rules[i].new_str);
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
