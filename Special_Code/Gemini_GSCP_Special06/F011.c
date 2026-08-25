// F011.c
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
    char *title;
    char *body;
} Draft;

int main() {
    Draft *arr = NULL;
    int count = 0;
    int cap = 0;
    char *disc_title = NULL;
    char *disc_body = NULL;
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
        if (strcmp(cmd, "CREATE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *body = p;
            size_t bl = strlen(body);
            while (bl > 0 && (body[bl-1] == '\n' || body[bl-1] == '\n')) {
                body[bl-1] = '\0';
                bl--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                Draft *narr = realloc(arr, cap * sizeof(Draft));
                if (narr) arr = narr;
            }
            if (count < cap) {
                arr[count].title = mystrdup(title);
                arr[count].body = mystrdup(body);
                count++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ' || *p == '	') p++;
            char *body = p;
            size_t bl = strlen(body);
            while (bl > 0 && (body[bl-1] == '\n' || body[bl-1] == '\n')) {
                body[bl-1] = '\0';
                bl--;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(arr[i].body);
                    arr[i].body = mystrdup(body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    free(disc_title);
                    free(disc_body);
                    disc_title = arr[i].title;
                    disc_body = arr[i].body;
                    for (int j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (disc_title) {
                if (count >= cap) {
                    cap = cap == 0 ? 8 : cap * 2;
                    Draft *narr = realloc(arr, cap * sizeof(Draft));
                    if (narr) arr = narr;
                }
                if (count < cap) {
                    arr[count].title = disc_title;
                    arr[count].body = disc_body;
                    count++;
                    disc_title = NULL;
                    disc_body = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ' || *p == '	') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '	' && *p != '\n' && *p != '\n') p++;
            if (*p) { *p = '\0'; p++; }
            for (int i = 0; i < count; i++) {
                if (strcmp(arr[i].title, title) == 0) {
                    printf("%s\n", arr[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].title, arr[i].body);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(arr[i].title);
        free(arr[i].body);
    }
    free(arr);
    free(disc_title);
    free(disc_body);
    return 0;
}
