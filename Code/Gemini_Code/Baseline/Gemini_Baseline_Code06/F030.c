// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *old_val;
    char *new_val;
} RenameRule;

int main() {
    RenameRule *rules = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char o[128], n[128];
            if (sscanf(line + 4, "%127s %127s", o, n) == 2) {
                rules = realloc(rules, (count + 1) * sizeof(RenameRule));
                rules[count].old_val = strdup(o);
                rules[count].new_val = strdup(n);
                count++;
            }
        } else if (strncmp(line, "APPLY ", 6) == 0) {
            char *text = line + 6;
            int applied = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(text, rules[i].old_val) == 0) {
                    printf("%s\n", rules[i].new_val);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *o = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(rules[i].old_val, o) == 0) {
                    free(rules[i].old_val);
                    free(rules[i].new_val);
                    for (int j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                    rules = realloc(rules, count * sizeof(RenameRule));
                    break;
                }
            }
        } else if (strncmp(line, "SWAP ", 5) == 0) {
            int idx1, idx2;
            if (sscanf(line + 5, "%d %d", &idx1, &idx2) == 2) {
                if (idx1 >= 0 && idx1 < count && idx2 >= 0 && idx2 < count) {
                    RenameRule temp = rules[idx1];
                    rules[idx1] = rules[idx2];
                    rules[idx2] = temp;
                }
            }
        } else if (strcmp(line, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s -> %s\n", rules[i].old_val, rules[i].new_val);
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
