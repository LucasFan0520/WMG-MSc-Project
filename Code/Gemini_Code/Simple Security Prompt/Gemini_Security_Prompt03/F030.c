// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *old_w;
    char *new_w;
} RenameRule;

char *read_line(FILE *fp) {
    size_t size = 32;
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

int main(void) {
    RenameRule *rules = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *old_w = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *new_w = p;
            rules = realloc(rules, (count + 1) * sizeof(RenameRule));
            rules[count].old_w = strdup(old_w);
            rules[count].new_w = strdup(new_w);
            count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            while (*p == ' ') p++;
            char *text = p;
            int applied = 0;
            for (int i = 0; i < count; i++) {
                if (rules[i].old_w && strcmp(text, rules[i].old_w) == 0) {
                    printf("%s\n", rules[i].new_w);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *old_w = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (rules[i].old_w && strcmp(rules[i].old_w, old_w) == 0) {
                    free(rules[i].old_w);
                    free(rules[i].new_w);
                    rules[i].old_w = NULL;
                    rules[i].new_w = NULL;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            while (*p == ' ') p++;
            char *i_s = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            int idxI = atoi(i_s);
            int idxJ = atoi(p);
            if (idxI >= 0 && idxI < count && idxJ >= 0 && idxJ < count) {
                RenameRule temp = rules[idxI];
                rules[idxI] = rules[idxJ];
                rules[idxJ] = temp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                if (rules[i].old_w) {
                    printf("%s -> %s\n", rules[i].old_w, rules[i].new_w);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (rules[i].old_w) {
            free(rules[i].old_w);
            free(rules[i].new_w);
        }
    }
    free(rules);
    return 0;
}
