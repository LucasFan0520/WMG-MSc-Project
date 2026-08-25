// F010.c
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
} IndexEntry;

int main() {
    IndexEntry *arr = NULL;
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
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                IndexEntry *narr = realloc(arr, cap * sizeof(IndexEntry));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].key = mystrdup(key);
                arr[count].value = mystrdup(value);
                count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
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
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0 && strcmp(arr[i].value, value) == 0) {
                    free(arr[i].key);
                    free(arr[i].value);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
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
        } else if (strcmp(cmd, "VALUES") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    printf("%s\n", arr[i].value);
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            char **seen = NULL;
            int seen_cnt = 0;
            for (int i = 0; i < count; i++) {
                int found = 0;
                for (int j = 0; j < seen_cnt; j++) {
                    if (strcmp(seen[j], arr[i].key) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%s\n", arr[i].key);
                    seen = realloc(seen, (seen_cnt + 1) * sizeof(char *));
                    if (seen) {
                        seen[seen_cnt] = arr[i].key;
                        seen_cnt++;
                    }
                }
            }
            free(seen);
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
