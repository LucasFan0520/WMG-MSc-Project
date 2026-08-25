// F004.c
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
    char *key;
    char *value;
} Var;

int main() {
    Var *arr = NULL;
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
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *value = p;
            size_t vl = strlen(value);
            while (vl > 0 && (value[vl-1] == '\n' || value[vl-1] == '\n')) {
                value[vl-1] = '\0';
                vl--;
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].value);
                    arr[i].value = mystrdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Var *narr = realloc(arr, cap * sizeof(Var));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].key = mystrdup(key);
                    arr[count].value = mystrdup(value);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].key);
                    free(arr[i].value);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *text = p;
            size_t tl = strlen(text);
            while (tl > 0 && (text[tl-1] == '\n' || text[tl-1] == '\n')) {
                text[tl-1] = '\0';
                tl--;
            }
            size_t ocap = 16;
            size_t olen = 0;
            char *obuf = malloc(ocap);
            if (obuf) {
                char *s = text;
                while (*s) {
                    if (*s == '{') {
                        char *start = s + 1;
                        char *end = start;
                        while (*end && *end != '}') end++;
                        if (*end == '}') {
                            size_t kl = end - start;
                            char *k = malloc(kl + 1);
                            if (k) {
                                strncpy(k, start, kl);
                                k[kl] = '\0';
                                char *val = NULL;
                                for (int i = 0; i < count; i++) {
                                    if (strcmp(arr[i].key, k) == 0) {
                                        val = arr[i].value;
                                        break;
                                    }
                                }
                                free(k);
                                if (val) {
                                    size_t vl = strlen(val);
                                    while (olen + vl >= ocap) {
                                        ocap *= 2;
                                        char *nobuf = realloc(obuf, ocap);
                                        if (nobuf) obuf = nobuf;
                                    }
                                    strcpy(obuf + olen, val);
                                    olen += vl;
                                    s = end + 1;
                                    continue;
                                }
                            }
                        }
                    }
                    if (olen + 1 >= ocap) {
                        ocap *= 2;
                        char *nobuf = realloc(obuf, ocap);
                        if (nobuf) obuf = nobuf;
                    }
                    obuf[olen++] = *s;
                    s++;
                }
                obuf[olen] = '\0';
                printf("%s\n", obuf);
                free(obuf);
            }
        } else if (strcmp(cmd, "VARS") == 0) {
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
