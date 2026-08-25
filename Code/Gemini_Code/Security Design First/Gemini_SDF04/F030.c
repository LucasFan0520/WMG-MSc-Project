// F030.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} RenameRule;

RenameRule *rules = NULL;
int count = 0;
int capacity = 0;

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ADD") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *new_s = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        RenameRule *tmp = realloc(rules, capacity * sizeof(RenameRule));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        rules = tmp;
                    }
                    rules[count].old_str = strdup(arg1);
                    rules[count].new_str = strdup(new_s);
                    if (rules[count].old_str && rules[count].new_str) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "APPLY") == 0) {
                int found = -1;
                char *pos = NULL;
                for (int i = 0; i < count; i++) {
                    pos = strstr(arg1, rules[i].old_str);
                    if (pos) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    size_t prefix_len = pos - arg1;
                    size_t old_len = strlen(rules[found].old_str);
                    size_t new_len = strlen(rules[found].new_str);
                    size_t suffix_len = strlen(pos + old_len);
                    char *res = malloc(prefix_len + new_len + suffix_len + 1);
                    if (res) {
                        strncpy(res, arg1, prefix_len);
                        res[prefix_len] = '\0';
                        strcat(res, rules[found].new_str);
                        strcat(res, pos + old_len);
                        printf("%s\n", res);
                        free(res);
                    }
                } else {
                    printf("%s\n", arg1);
                }
            } else if (strcmp(cmd, "DELETE") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(rules[i].old_str, arg1) == 0) {
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
            } else if (strcmp(cmd, "SWAP") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    int idx1 = atoi(arg1);
                    int idx2 = atoi(space2 + 1);
                    if (idx1 >= 0 && idx1 < count && idx2 >= 0 && idx2 < count && idx1 != idx2) {
                        RenameRule tmp = rules[idx1];
                        rules[idx1] = rules[idx2];
                        rules[idx2] = tmp;
                    }
                }
            }
        } else {
            if (strcmp(cmd, "RULES") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s -> %s\n", rules[i].old_str, rules[i].new_str);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
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