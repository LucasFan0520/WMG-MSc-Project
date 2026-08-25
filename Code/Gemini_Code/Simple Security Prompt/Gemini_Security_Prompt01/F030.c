// F030.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_str;
    char *new_str;
} RenameRule;

int main() {
    RenameRule *rules = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *old_p = args;
            char *space2 = strchr(args, ' ');
            char *new_p = "";
            if (space2) {
                *space2 = '\0';
                new_p = space2 + 1;
                while (*new_p == ' ') new_p++;
            }
            rules = realloc(rules, (count + 1) * sizeof(RenameRule));
            if (!rules) return 1;
            rules[count].old_str = strdup(old_p);
            rules[count].new_str = strdup(new_p);
            count++;
        } else if (strcmp(cmd, "APPLY") == 0 && args) {
            int rule_found = -1;
            char *pos = NULL;
            for (int i = 0; i < count; i++) {
                pos = strstr(args, rules[i].old_str);
                if (pos != NULL) {
                    rule_found = i;
                    break;
                }
            }
            if (rule_found != -1) {
                size_t prefix_len = pos - args;
                size_t old_len = strlen(rules[rule_found].old_str);
                printf("%.*s%s%s\n", (int)prefix_len, args, rules[rule_found].new_str, pos + old_len);
            } else {
                printf("%s\n", args);
            }
        } else if (strcmp(cmd, "DELETE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(rules[i].old_str, args) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (int j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                    rules = realloc(rules, count * sizeof(RenameRule));
                    if (count > 0 && !rules) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0 && args) {
            char *i_str = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int idx1 = atoi(i_str);
                int idx2 = atoi(space2 + 1);
                if (idx1 >= 0 && idx1 < count && idx2 >= 0 && idx2 < count) {
                    RenameRule temp = rules[idx1];
                    rules[idx1] = rules[idx2];
                    rules[idx2] = temp;
                }
            }
        } else if (strcmp(cmd, "RULES") == 0) {
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
