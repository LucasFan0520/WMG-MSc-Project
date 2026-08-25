// F033.c
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
    char *id;
    char *name;
    char *dept;
    char *title;
} Emp;

int main() {
    Emp *arr = NULL;
    int count = 0;
    int cap = 0;
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
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *dept = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            size_t tl = strlen(title);
            while (tl > 0 && (title[tl-1] == '\r' || title[tl-1] == '\n')) {
                title[tl-1] = '\0';
                tl--;
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Emp *narr = realloc(arr, cap * sizeof(Emp));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].id = mystrdup(id);
                    arr[count].name = mystrdup(name);
                    arr[count].dept = mystrdup(dept);
                    arr[count].title = mystrdup(title);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *dept = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            size_t tl = strlen(title);
            while (tl > 0 && (title[tl-1] == '\r' || title[tl-1] == '\n')) {
                title[tl-1] = '\0';
                tl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].name);
                    free(arr[i].dept);
                    free(arr[i].title);
                    arr[i].name = mystrdup(name);
                    arr[i].dept = mystrdup(dept);
                    arr[i].title = mystrdup(title);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    free(arr[i].id);
                    free(arr[i].name);
                    free(arr[i].dept);
                    free(arr[i].title);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *id = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].id, id) == 0) {
                    printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].dept, arr[i].title);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", arr[i].id, arr[i].name, arr[i].dept, arr[i].title);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].id);
        free(arr[i].name);
        free(arr[i].dept);
        free(arr[i].title);
    }
    free(arr);
    return 0;
}
