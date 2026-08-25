// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *old_str;
    char *new_str;
} RenameRule;

int main(void) {
    RenameRule *rules = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *old_s = p;
            while (*old_s == ' ') old_s++;
            char *old_end = old_s;
            while (*old_end && *old_end != ' ') old_end++;
            char *new_s = old_end;
            if (*old_end != '\0') {
                *old_end = '\0';
                new_s++;
            }
            while (*new_s == ' ') new_s++;
            char *new_end = new_s;
            while (*new_end && *new_end != ' ') new_end++;
            *new_end = '\0';
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                RenameRule *new_r = realloc(rules, capacity * sizeof(RenameRule));
                if (new_r) rules = new_r;
            }
            rules[count].old_str = mystrdup(old_s);
            rules[count].new_str = mystrdup(new_s);
            count++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = p;
            while (*text == ' ') text++;
            char *text_end = text;
            while (*text_end && *text_end != ' ') text_end++;
            *text_end = '\0';
            int rule_applied = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(text, rules[i].old_str) == 0) {
                    printf("%s\n", rules[i].new_str);
                    rule_applied = 1;
                    break;
                }
            }
            if (!rule_applied) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old_s = p;
            while (*old_s == ' ') old_s++;
            char *old_end = old_s;
            while (*old_end && *old_end != ' ') old_end++;
            *old_end = '\0';
            for (size_t i = 0; i < count; ) {
                if (strcmp(rules[i].old_str, old_s) == 0) {
                    free(rules[i].old_str);
                    free(rules[i].new_str);
                    for (size_t j = i; j < count - 1; j++) {
                        rules[j] = rules[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i_s = p;
            while (*i_s == ' ') i_s++;
            char *i_end = i_s;
            while (*i_end && *i_end != ' ') i_end++;
            char *j_s = i_end;
            if (*i_end != '\0') {
                *i_end = '\0';
                j_s++;
            }
            while (*j_s == ' ') j_s++;
            char *j_end = j_s;
            while (*j_end && *j_end != ' ') j_end++;
            *j_end = '\0';
            int idx_i = atoi(i_s);
            int idx_j = atoi(j_s);
            if (idx_i >= 0 && (size_t)idx_i < count && idx_j >= 0 && (size_t)idx_j < count) {
                RenameRule temp = rules[idx_i];
                rules[idx_i] = rules[idx_j];
                rules[idx_j] = temp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", rules[i].old_str, rules[i].new_str);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(rules[i].old_str);
        free(rules[i].new_str);
    }
    free(rules);
    return 0;
}
