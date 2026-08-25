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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *old_str;
    char *new_str;
} Rule;

int main() {
    Rule *arr = NULL;
    int count = 0;
    int cap = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *old_str = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *new_str = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Rule *narr = realloc(arr, cap * sizeof(Rule));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].old_str = mystrdup(old_str);
                arr[count].new_str = mystrdup(new_str);
                count++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *text = p;
            size_t tl = strlen(text);
            while (tl > 0 && (text[tl-1] == '\r' || text[tl-1] == '\n')) {
                text[tl-1] = '\0';
                tl--;
            }
            int matched = -1;
            char *pos = NULL;
            for (int i = 0; i < count; i++) {
                pos = strstr(text, arr[i].old_str);
                if (pos) {
                    matched = i;
                    break;
                }
            }
            if (matched != -1) {
                size_t old_len = strlen(arr[matched].old_str);
                size_t new_len = strlen(arr[matched].new_str);
                size_t prefix_len = pos - text;
                char *suffix = pos + old_len;
                size_t suffix_len = strlen(suffix);
                char *res = malloc(prefix_len + new_len + suffix_len + 1);
                if (res) {
                    strncpy(res, text, prefix_len);
                    strcpy(res + prefix_len, arr[matched].new_str);
                    strcpy(res + prefix_len + new_len, suffix);
                    printf("%s\n", res);
                    free(res);
                }
            } else {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *old_str = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].old_str, old_str) == 0) {
                    free(arr[i].old_str);
                    free(arr[i].new_str);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *i_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *j_s = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int idx_i = atoi(i_s);
            int idx_j = atoi(j_s);
            if (idx_i >= 0 && idx_i < count && idx_j >= 0 && idx_j < count && idx_i != idx_j) {
                Rule tmp = arr[idx_i];
                arr[idx_i] = arr[idx_j];
                arr[idx_j] = tmp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s -> %s\n", arr[i].old_str, arr[i].new_str);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].old_str);
        free(arr[i].new_str);
    }
    free(arr);
    return 0;
}
