// F029.c
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
    char *key;
    char *value;
} Record;

int main() {
    Record *arr = NULL;
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
            char *value = get_rest(&p);
            if (key) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Record *narr = realloc(arr, cap * sizeof(Record));
                    if (narr) arr = narr;
                }
                arr[count].key = strdup(key);
                arr[count].value = strdup(value);
                count++;
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (int i = 0; i < count; i++) {
                for (int j = i + 1; j < count; ) {
                    if (strcmp(arr[i].key, arr[j].key) == 0) {
                        free(arr[j].key);
                        free(arr[j].value);
                        for (int k = j; k < count - 1; k++) {
                            arr[k] = arr[k + 1];
                        }
                        count--;
                    } else {
                        j++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = get_token(&p);
            if (key) {
                for (int i = 0; i < count; ) {
                    if (strcmp(arr[i].key, key) == 0) {
                        free(arr[i].key);
                        free(arr[i].value);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = get_token(&p);
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].key, key) == 0) {
                        printf("%s\n", arr[i].value);
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].key, arr[i].value);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].key);
        free(arr[i].value);
    }
    free(arr);
    return 0;
}
