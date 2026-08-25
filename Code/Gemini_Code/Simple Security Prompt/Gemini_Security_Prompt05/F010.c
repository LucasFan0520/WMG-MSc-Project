// F010.c
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

typedef struct {
    char *key;
    char **vals;
    int count;
    int cap;
} KeyIndex;

int main() {
    KeyIndex *arr = NULL;
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
            char *key = get_token(&p);
            char *val = get_token(&p);
            if (key && val) {
                int kidx = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        kidx = i;
                        break;
                    }
                }
                if (kidx == -1) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        KeyIndex *narr = realloc(arr, cap * sizeof(KeyIndex));
                        if (narr) arr = narr;
                    }
                    arr[count].key = strdup(key);
                    arr[count].vals = NULL;
                    arr[count].count = 0;
                    arr[count].cap = 0;
                    kidx = count;
                    count++;
                }
                KeyIndex *ki = &arr[kidx];
                if (ki->count >= ki->cap) {
                    ki->cap = ki->cap == 0 ? 4 : ki->cap * 2;
                    char **nvals = realloc(ki->vals, ki->cap * sizeof(char *));
                    if (nvals) ki->vals = nvals;
                }
                ki->vals[ki->count] = strdup(val);
                ki->count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = get_token(&p);
            char *val = get_token(&p);
            if (key && val) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        for (int j = 0; j < arr[i].count; j++) {
                            if (strcmp(arr[i].vals[j], val) == 0) {
                                free(arr[i].vals[j]);
                                for (int k = j; k < arr[i].count - 1; k++) {
                                    arr[i].vals[k] = arr[i].vals[k + 1];
                                }
                                arr[i].count--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = get_token(&p);
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        free(arr[i].key);
                        for (int j = 0; j < arr[i].count; j++) {
                            free(arr[i].vals[j]);
                        }
                        free(arr[i].vals);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = get_token(&p);
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        for (int j = 0; j < arr[i].count; j++) {
                            printf("%s%s", arr[i].vals[j], j == arr[i].count - 1 ? "" : " ");
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", arr[i].key);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].key);
        for (int j = 0; j < arr[i].count; j++) {
            free(arr[i].vals[j]);
        }
        free(arr[i].vals);
    }
    free(arr);
    return 0;
}
