// F030.c
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            char old_val[256], new_val[256];
            if (sscanf(ptr, "%255s %255s", old_val, new_val) == 2) {
                rules = realloc(rules, (count + 1) * sizeof(RenameRule));
                rules[count].old_str = strdup(old_val);
                rules[count].new_str = strdup(new_val);
                count++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *found = NULL;
            int r_idx = -1;
            for (int i = 0; i < count; i++) {
                found = strstr(ptr, rules[i].old_str);
                if (found) {
                    r_idx = i;
                    break;
                }
            }
            if (r_idx != -1 && found) {
                int prefix_len = found - ptr;
                int old_len = strlen(rules[r_idx].old_str);
                printf("%.*s%s%s\n", prefix_len, ptr, rules[r_idx].new_str, found + old_len);
            } else {
                printf("%s\n", ptr);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char old_val[256];
            if (sscanf(ptr, "%255s", old_val) > 0) {
                int i = 0;
                while (i < count) {
                    if (strcmp(rules[i].old_str, old_val) == 0) {
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
                if (count > 0) {
                    rules = realloc(rules, count * sizeof(RenameRule));
                } else {
                    free(rules);
                    rules = NULL;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            int i = -1, j = -1;
            if (sscanf(ptr, "%d %d", &i, &j) == 2) {
                if (i >= 0 && i < count && j >= 0 && j < count) {
                    RenameRule temp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = temp;
                }
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d: %s -> %s\n", i, rules[i].old_str, rules[i].new_str);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    return 0;
}
