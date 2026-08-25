// F002.c
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
    char *tag;
    char *message;
} Msg;

int main() {
    Msg *arr = NULL;
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
            char *tag = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *message = p;
            size_t ml = strlen(message);
            while (ml > 0 && (message[ml-1] == '\n' || message[ml-1] == '\n')) {
                message[ml-1] = '\0';
                ml--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Msg *narr = realloc(arr, cap * sizeof(Msg));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].tag = mystrdup(tag);
                arr[count].message = mystrdup(message);
                count++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *tag1 = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *tag2 = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *newtag = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            char *m1 = NULL;
            char *m2 = NULL;
            for (int i = 0; i < count; i++) {
                if (!m1 && strcmp(arr[i].tag, tag1) == 0) {
                    m1 = arr[i].message;
                }
                if (!m2 && strcmp(arr[i].tag, tag2) == 0) {
                    m2 = arr[i].message;
                }
            }
            if (m1 && m2) {
                size_t l1 = strlen(m1);
                size_t l2 = strlen(m2);
                char *nm = malloc(l1 + l2 + 2);
                if (nm) {
                    strcpy(nm, m1);
                    nm[l1] = ' ';
                    strcpy(nm + l1 + 1, m2);
                    if (count >= cap) {
                        cap = cap == 0 ? 8 : cap * 2;
                        Msg *narr = realloc(arr, cap * sizeof(Msg));
                        if (narr) arr = narr;
                    }
                    if (count < cap) {
                        arr[count].tag = mystrdup(newtag);
                        arr[count].message = nm;
                        count++;
                    } else {
                        free(nm);
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; ) {
                if (strcmp(arr[i].tag, tag) == 0) {
                    free(arr[i].tag);
                    free(arr[i].message);
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *tag = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].tag, tag) == 0) {
                    char *s = arr[i].message;
                    while (*s) {
                        if (*s == ' ') putchar('_');
                        else putchar(*s);
                        s++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].tag, arr[i].message);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].tag);
        free(arr[i].message);
    }
    free(arr);
    return 0;
}
