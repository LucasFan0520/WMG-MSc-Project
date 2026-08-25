// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

int main(void) {
    Job *queue = NULL;
    int count = 0;
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
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ENQUEUE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *payload = p;
            queue = realloc(queue, (count + 1) * sizeof(Job));
            queue[count].name = strdup(name);
            queue[count].payload = strdup(payload);
            count++;
        } else if (strcmp(cmd, "CLONE") == 0) {
            while (*p == ' ') p++;
            char *oldName = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *newName = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (queue[i].name && strcmp(queue[i].name, oldName) == 0) {
                    queue = realloc(queue, (count + 1) * sizeof(Job));
                    queue[count].name = strdup(newName);
                    queue[count].payload = strdup(queue[i].payload);
                    count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (queue[i].name && strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].payload);
                    queue[i].name = NULL;
                    queue[i].payload = NULL;
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < count; i++) {
                if (queue[i].name && strcmp(queue[i].name, name) == 0) {
                    printf("%s\n", queue[i].payload);
                    free(queue[i].name);
                    free(queue[i].payload);
                    queue[i].name = NULL;
                    queue[i].payload = NULL;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                if (queue[i].name) {
                    printf("%s\n", queue[i].name);
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        if (queue[i].name) {
            free(queue[i].name);
            free(queue[i].payload);
        }
    }
    free(queue);
    return 0;
}
