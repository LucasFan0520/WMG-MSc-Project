// F006.c
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
    char *item;
    long long count;
} InvItem;

int main() {
    InvItem *arr = NULL;
    int count = 0;
    int cap = 0;
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        char *p = line;
        while (*p == ' ' || *p == '	' || *p == '\n' || *p == '\n') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *item = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *cnt_s = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            long long cnt = strtoll(cnt_s, NULL, 10);
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    arr[i].count += cnt;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    InvItem *narr = realloc(arr, cap * sizeof(InvItem));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].item = mystrdup(item);
                    arr[count].count = cnt;
                    count++;
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *item = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *cnt_s = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            long long cnt = strtoll(cnt_s, NULL, 10);
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    arr[i].count -= cnt;
                    if (arr[i].count < 0) arr[i].count = 0;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *item = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
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
        } else if (strcmp(cmd, "COUNT") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *item = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            long long cnt = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].item, item) == 0) {
                    cnt = arr[i].count;
                    break;
                }
            }
            printf("%lld\n", cnt);
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
