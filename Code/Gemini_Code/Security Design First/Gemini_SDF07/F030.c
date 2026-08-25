/* F030.c */
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
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            char *o = NULL;
            char *n = NULL;
            if (space) {
                *space = '\0';
                o = args;
                n = space + 1;
            } else {
                o = args;
                n = "";
            }
            char *n_old = strdup(o);
            char *n_new = strdup(n);
            if (n_old && n_new) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    RenameRule *tmp = realloc(rules, new_cap * sizeof(RenameRule));
                    if (tmp) {
                        rules = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_old); free(n_new);
                        if (space) *space = ' ';
                        continue;
                    }
                }
                rules[count].old_str = n_old;
                rules[count].new_str = n_new;
                count++;
            } else {
                free(n_old); free(n_new);
            }
            if (space) *space = ' ';
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            int applied = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(text, rules[i].old_str) == 0) {
                    printf("%s\n", rules[i].new_str);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *o = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(rules[i].old_str, o) == 0) {
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
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                int i = atoi(args);
                int j = atoi(space + 1);
                if (i >= 0 && i < count && j >= 0 && j < count && i != j) {
                    RenameRule tmp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = tmp;
                }
                *space = ' ';
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", rules[i].old_str, rules[i].new_str);
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
