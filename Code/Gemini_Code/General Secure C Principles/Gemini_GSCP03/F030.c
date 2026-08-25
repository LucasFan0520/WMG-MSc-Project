// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *old_val;
    char *new_val;
} Rule;

int main() {
    Rule *items = NULL;
    int count = 0;
    int cap = 0;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s -> %s\n", items[i].old_val, items[i].new_val);
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char *old_val = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *new_val = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                Rule *nb = realloc(items, cap * sizeof(Rule));
                if (nb) items = nb;
            }
            items[count].old_val = strdup(old_val);
            items[count].new_val = strdup(new_val);
            count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old_val = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (strcmp(items[i].old_val, old_val) == 0) {
                    free(items[i].old_val);
                    free(items[i].new_val);
                    for (int j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *j_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int idx_i = atoi(i_str);
            int idx_j = atoi(j_str);
            if (idx_i >= 0 && idx_i < count && idx_j >= 0 && idx_j < count) {
                Rule tmp = items[idx_i];
                items[idx_i] = items[idx_j];
                items[idx_j] = tmp;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int applied = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(text, items[i].old_val) == 0) {
                    printf("%s\n", items[i].new_val);
                    applied = 1;
                    break;
                }
            }
            if (!applied) {
                printf("%s\n", text);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(items[i].old_val);
        free(items[i].new_val);
    }
    free(items);
    return 0;
}
