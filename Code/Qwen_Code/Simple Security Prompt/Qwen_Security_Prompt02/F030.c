// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_name;
    char *new_name;
} Rule;

Rule *rules = NULL;
int r_count = 0;

int find_rule(const char *old_name) {
    for (int i = 0; i < r_count; i++) {
        if (strcmp(rules[i].old_name, old_name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char old_name[1024];
    char new_name[1024];
    char text[10240];
    int i_idx;
    int j_idx;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", old_name, new_name);
            rules = realloc(rules, sizeof(Rule) * (r_count + 1));
            rules[r_count].old_name = strdup(old_name);
            rules[r_count].new_name = strdup(new_name);
            r_count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            fgets(text, sizeof(text), stdin);
            if (text[0] == ' ') {
                memmove(text, text + 1, strlen(text));
            }
            text[strcspn(text, "\n")] = '\0';
            int applied = 0;
            for (int i = 0; i < r_count; i++) {
                char *pos = strstr(text, rules[i].old_name);
                if (pos != NULL) {
                    size_t prefix_len = (size_t)(pos - text);
                    size_t old_len = strlen(rules[i].old_name);
                    size_t suffix_len = strlen(pos + old_len);
                    size_t new_len = strlen(rules[i].new_name);
                    size_t total = prefix_len + new_len + suffix_len + 1;
                    char *result = malloc(total);
                    memcpy(result, text, prefix_len);
                    memcpy(result + prefix_len, rules[i].new_name, new_len);
                    memcpy(result + prefix_len + new_len, pos + old_len, suffix_len);
                    result[prefix_len + new_len + suffix_len] = '\0';
                    printf("%s\n", result);
                    free(result);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", old_name);
            int idx = find_rule(old_name);
            if (idx != -1) {
                free(rules[idx].old_name);
                free(rules[idx].new_name);
                for (int i = idx; i < r_count - 1; i++) {
                    rules[i] = rules[i + 1];
                }
                r_count--;
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            scanf("%d %d", &i_idx, &j_idx);
            if (i_idx >= 0 && i_idx < r_count && j_idx >= 0 && j_idx < r_count) {
                Rule tmp = rules[i_idx];
                rules[i_idx] = rules[j_idx];
                rules[j_idx] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < r_count; i++) {
                printf("%s %s\n", rules[i].old_name, rules[i].new_name);
            }
        }
    }

    for (int i = 0; i < r_count; i++) {
        free(rules[i].old_name);
        free(rules[i].new_name);
    }
    free(rules);

    return 0;
}