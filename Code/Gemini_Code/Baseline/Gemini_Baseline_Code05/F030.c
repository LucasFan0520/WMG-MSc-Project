// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_val;
    char *new_val;
} RenameRule;

RenameRule *rules = NULL;
int count = 0;
int capacity = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *new_val = sp + 1;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    rules = realloc(rules, capacity * sizeof(RenameRule));
                }
                rules[count].old_val = strdup(p1);
                rules[count].new_val = strdup(new_val);
                count++;
            }
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            int matched = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(text, rules[i].old_val) == 0) {
                    printf("%s\n", rules[i].new_val);
                    matched = 1;
                    break;
                }
            }
            if (!matched) {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *old_val = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(rules[i].old_val, old_val) == 0) {
                    free(rules[i].old_val);
                    free(rules[i].new_val);
                    for (int j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            char *p1 = line + 5;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                int i = atoi(p1);
                int j = atoi(sp + 1);
                if (i >= 0 && i < count && j >= 0 && j < count) {
                    RenameRule temp = rules[i];
                    rules[i] = rules[j];
                    rules[j] = temp;
                }
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", rules[i].old_val, rules[i].new_val);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(rules[i].old_val);
        free(rules[i].new_val);
    }
    free(rules);
    return 0;
}
