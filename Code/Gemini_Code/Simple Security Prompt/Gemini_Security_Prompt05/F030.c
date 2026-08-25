// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    int cap = 16;
    int len = 0;
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

char *get_token(char **p) {
    while (**p == ' ') (*p)++;
    if (**p == '\0') return NULL;
    char *start = *p;
    while (**p && **p != ' ') (*p)++;
    if (**p) {
        **p = '\0';
        (*p)++;
    }
    return start;
}

char *get_rest(char **p) {
    while (**p == ' ') (*p)++;
    return **p ? *p : "";
}

typedef struct {
    char *old_val;
    char *new_val;
} Rule;

int main() {
    Rule *arr = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        char *cmd = get_token(&p);
        if (!cmd || strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *old = get_token(&p);
            char *nval = get_token(&p);
            if (old && nval) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Rule *narr = realloc(arr, cap * sizeof(Rule));
                    if (narr) arr = narr;
                }
                arr[count].old_val = strdup(old);
                arr[count].new_val = strdup(nval);
                count++;
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = get_rest(&p);
            int matched = 0;
            for (int i = 0; i < count; i++) {
                if (strstr(text, arr[i].old_val) != NULL) {
                    char *old = arr[i].old_val;
                    char *nval = arr[i].new_val;
                    int olen = strlen(old);
                    int nlen = strlen(nval);
                    char *res = malloc(4096);
                    res[0] = '\0';
                    char *curr = text;
                    char *next;
                    while ((next = strstr(curr, old)) != NULL) {
                        strncat(res, curr, next - curr);
                        strcat(res, nval);
                        curr = next + olen;
                    }
                    strcat(res, curr);
                    printf("%s\n", res);
                    free(res);
                    matched = 1;
                    break;
                }
            }
            if (!matched) {
                printf("%s\n", text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old = get_token(&p);
            if (old) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].old_val, old) == 0) {
                        free(arr[i].old_val);
                        free(arr[i].new_val);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i_s = get_token(&p);
            char *j_s = get_token(&p);
            if (i_s && j_s) {
                int idx_i = atoi(i_s);
                int idx_j = atoi(j_s);
                if (idx_i >= 0 && idx_i < count && idx_j >= 0 && idx_j < count) {
                    Rule temp = arr[idx_i];
                    arr[idx_i] = arr[idx_j];
                    arr[idx_j] = temp;
                }
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].old_val, arr[i].new_val);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].old_val);
        free(arr[i].new_val);
    }
    free(arr);
    return 0;
}
