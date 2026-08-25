// F006.c
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
    char *item;
    long long count;
} InvItem;

int main() {
    InvItem *arr = NULL;
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
            char *item = get_token(&p);
            char *cnt_s = get_token(&p);
            if (item && cnt_s) {
                long long amt = atoll(cnt_s);
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].item, item) == 0) {
                        arr[i].count += amt;
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        InvItem *narr = realloc(arr, cap * sizeof(InvItem));
                        if (narr) arr = narr;
                    }
                    arr[count].item = strdup(item);
                    arr[count].count = amt;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *item = get_token(&p);
            char *cnt_s = get_token(&p);
            if (item && cnt_s) {
                long long amt = atoll(cnt_s);
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].item, item) == 0) {
                        arr[i].count -= amt;
                        if (arr[i].count < 0) arr[i].count = 0;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = get_token(&p);
            if (item) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].item, item) == 0) {
                        free(arr[i].item);
                        for (int j = i; j < count - 1; j++) {
                            arr[j] = arr[j + 1];
                        }
                        count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *item = get_token(&p);
            if (item) {
                long long cur = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(arr[i].item, item) == 0) {
                        cur = arr[i].count;
                        break;
                    }
                }
                printf("%lld\n", cur);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %lld\n", arr[i].item, arr[i].count);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].item);
    }
    free(arr);
    return 0;
}
