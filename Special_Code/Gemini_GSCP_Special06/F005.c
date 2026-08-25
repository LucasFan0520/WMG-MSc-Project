// F005.c
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
            buf= nb;
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

int main() {
    char **arr = NULL;
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
        if (strcmp(cmd, "INSERT") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *text = p;
            size_t tl = strlen(text);
            while (tl > 0 && (text[tl-1] == '\n' || text[tl-1] == '\n')) {
                text[tl-1] = '\0';
                tl--;
            }
            int idx = atoi(idx_s);
            if (idx >= 0 && idx <= count) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    char **narr = realloc(arr, cap * sizeof(char *));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    for (int j = count; j > idx; j--) {
                        arr[j] = arr[j - 1];
                    }
                    arr[idx] = mystrdup(text);
                    count++;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *text = p;
            size_t tl = strlen(text);
            while (tl > 0 && (text[tl-1] == '\n' || text[tl-1] == '\n')) {
                text[tl-1] = '\0';
                tl--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                char **narr = realloc(arr, cap * sizeof(char *));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count] = mystrdup(text);
                count++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            int idx = atoi(idx_s);
            if (idx >= 0 && idx < count) {
                free(arr[idx]);
                for (int j = idx; j < count - 1; j++) {
                    arr[j] = arr[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *idx_s = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *text = p;
            size_t tl = strlen(text);
            while (tl > 0 && (text[tl-1] == '\n' || text[tl-1] == '\n')) {
                text[tl-1] = '\0';
                tl--;
            }
            int idx = atoi(idx_s);
            if (idx >= 0 && idx < count) {
                free(arr[idx]);
                arr[idx] = mystrdup(text);
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                char *s = arr[i];
                while (*s) {
                    if (*s == ' ') putchar('_');
                    else putchar(*s);
                    s++;
                }
                putchar('\n');
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i]);
    }
    free(arr);
    return 0;
}
