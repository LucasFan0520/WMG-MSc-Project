/* F035.c */
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *payload;
} Job;

int main(void) {
    Job *queue = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ENQUEUE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *payload = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Job *nj = realloc(queue, cap * sizeof(Job));
                if (nj) queue = nj;
            }
            char *nn = mystrdup(name);
            char *np = mystrdup(payload);
            if (nn && np) {
                queue[count].name = nn;
                queue[count].payload = np;
                count++;
            } else {
                free(nn); free(np);
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            while (*p == ' ') p++;
            char *oldName = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *newName = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(queue[i].name, oldName) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Job *nj = realloc(queue, cap * sizeof(Job));
                    if (nj) queue = nj;
                }
                char *nn = mystrdup(newName);
                char *np = mystrdup(queue[found].payload);
                if (nn && np) {
                    queue[count].name = nn;
                    queue[count].payload = np;
                    count++;
                } else {
                    free(nn); free(np);
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            size_t i = 0;
            while (i < count) {
                if (strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') { *p = '\0'; p++; }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s\n", queue[found].payload);
                free(queue[found].name);
                free(queue[found].payload);
                for (size_t j = (size_t)found; j < count - 1; j++) {
                    queue[j] = queue[j + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", queue[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].payload);
    }
    free(queue);
    return 0;
}
