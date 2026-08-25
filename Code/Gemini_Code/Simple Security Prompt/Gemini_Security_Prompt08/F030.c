// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *old_str;
    char *new_str;
} RenameRule;

int main(void) {
    RenameRule *list = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') { free(line); continue; }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) { free(line); break; }
        while (*p == ' ') p++;
        char *arg1 = p;
        if (strcmp(cmd, "ADD") == 0) {
            char *old_str = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *new_str = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                RenameRule *nb = realloc(list, cap * sizeof(RenameRule));
                if (nb) list = nb;
            }
            if (count < cap) {
                list[count].old_str = strdup(old_str);
                list[count].new_str = strdup(new_str);
                count++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *res = strdup(arg1);
            for (size_t i = 0; i < count; i++) {
                char *loc = strstr(res, list[i].old_str);
                if (loc) {
                    size_t old_len = strlen(list[i].old_str);
                    size_t new_len = strlen(list[i].new_str);
                    size_t res_len = strlen(res);
                    size_t prefix_len = loc - res;
                    char *nb = malloc(prefix_len + new_len + strlen(loc + old_len) + 1);
                    if (nb) {
                        memcpy(nb, res, prefix_len);
                        memcpy(nb + prefix_len, list[i].new_str, new_len);
                        strcpy(nb + prefix_len + new_len, loc + old_len);
                        free(res);
                        res = nb;
                    }
                    break;
                }
            }
            printf("%s\n", res);
            free(res);
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(list[i].old_str, arg1) == 0) {
                    free(list[i].old_str);
                    free(list[i].new_str);
                    for (size_t j = i; j < count - 1; j++) {
                        list[j] = list[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *idx1_str = arg1;
            p = arg1;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int i = atoi(idx1_str);
            int j = atoi(p);
            if (i >= 0 && (size_t)i < count && j >= 0 && (size_t)j < count) {
                RenameRule temp = list[i];
                list[i] = list[j];
                list[j] = temp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s -> %s\n", list[i].old_str, list[i].new_str);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(list[i].old_str);
        free(list[i].new_str);
    }
    free(list);
    return 0;
}
