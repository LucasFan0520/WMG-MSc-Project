// F017.c
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
} ConfigEntry;

int main(int argc, char **argv) {
    ConfigEntry *arr = NULL;
    int count = 0;
    int cap = 0;
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *fline;
            while ((fline = read_line(f)) != NULL) {
                char *eq = strchr(fline, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = fline;
                    char *value = eq + 1;
                    size_t vl = strlen(value);
                    while (vl > 0 && (value[vl-1] == '\r' || value[vl-1] == '\n')) {
                        value[vl-1] = '\0';
                        vl--;
                    }
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        ConfigEntry *narr = realloc(arr, cap * sizeof(ConfigEntry));
                        if (narr) arr = narr;
                    }
                    if (count < cap) {
                        arr[count].key = mystrdup(key);
                        arr[count].value = mystrdup(value);
                        count++;
                    }
                }
                free(fline);
            }
            fclose(f);
        }
    }
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
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *value = p;
            size_t vl = strlen(value);
            while (vl > 0 && (value[vl-1] == '\r' || value[vl-1] == '\n')) {
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
                    ConfigEntry *narr = realloc(arr, cap * sizeof(ConfigEntry));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].key = mystrdup(key);
                    arr[count].value = mystrdup(value);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            size_t kl = strlen(key);
            while (kl > 0 && (key[kl-1] == '\r' || key[kl-1] == '\n')) {
                key[kl-1] = '\0';
                kl--;
            }
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
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            size_t kl = strlen(key);
            while (kl > 0 && (key[kl-1] == '\r' || key[kl-1] == '\n')) {
                key[kl-1] = '\0';
                kl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    printf("%s\n", arr[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", arr[i].key, arr[i].value);
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
