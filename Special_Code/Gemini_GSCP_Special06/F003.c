// F003.c
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
    char *name;
    char *expansion;
} Alias;

int main() {
    Alias *arr = NULL;
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
        if (strcmp(cmd, "DEFINE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *expansion = p;
            size_t el = strlen(expansion);
            while (el > 0 && (expansion[el-1] == '\n' || expansion[el-1] == '\n')) {
                expansion[el-1] = '\0';
                el--;
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].expansion);
                    arr[i].expansion = mystrdup(expansion);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Alias *narr = realloc(arr, cap * sizeof(Alias));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].name = mystrdup(name);
                    arr[count].expansion = mystrdup(expansion);
                    count++;
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    char *s = arr[i].expansion;
                    while (*s) {
                        if (*s == ' ') putchar('_');
                        else putchar(*s);
                        s++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *old_name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *new_name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, old_name) == 0) {
                    free(arr[i].name);
                    arr[i].name = mystrdup(new_name);
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    free(arr[i].expansion);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].expansion);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].expansion);
    }
    free(arr);
    return 0;
}
