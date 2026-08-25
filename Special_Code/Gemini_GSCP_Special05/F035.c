// F035.c
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
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
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
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ENQUEUE ", 8) == 0) {
            char *p = line + 8;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *payload = "";
            if (*p == ' ') {
                *p = '\0';
                payload = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Job *nb = realloc(queue, cap * sizeof(Job));
                if (!nb) {
                    free(line);
                    break;
                }
                queue = nb;
            }
            queue[count].name = safe_dup(name);
            queue[count].payload = safe_dup(payload);
            count++;
        } else if (strncmp(line, "CLONE ", 6) == 0) {
            char *p = line + 6;
            char *oldName = p;
            while (*p && *p != ' ') p++;
            char *newName = "";
            if (*p == ' ') {
                *p = '\0';
                newName = p + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(queue[i].name, oldName) == 0) {
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        Job *nb = realloc(queue, cap * sizeof(Job));
                        if (!nb) break;
                        queue = nb;
                    }
                    queue[count].name = safe_dup(newName);
                    queue[count].payload = safe_dup(queue[i].payload);
                    count++;
                    break;
                }
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "RUN ", 4) == 0) {
            char *name = line + 4;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    printf("%s\n", queue[i].payload);
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (size_t j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
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
