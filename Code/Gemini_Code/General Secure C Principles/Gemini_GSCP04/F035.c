// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *payload;
} Job;

int main(void) {
    Job *queue = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ENQUEUE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *payload = p;
            if (*name != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Job *new_queue = realloc(queue, capacity * sizeof(Job));
                    if (new_queue) queue = new_queue;
                }
                if (count < capacity) {
                    queue[count].name = strdup(name);
                    queue[count].payload = strdup(payload);
                    count++;
                }
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *oldName = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *newName = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(queue[i].name, oldName) == 0 && *newName != '\0') {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Job *new_queue = realloc(queue, capacity * sizeof(Job));
                        if (new_queue) queue = new_queue;
                    }
                    if (count < capacity) {
                        queue[count].name = strdup(newName);
                        queue[count].payload = strdup(queue[i].payload);
                        count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
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
        } else if (strcmp(cmd, "RUN") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
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
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", queue[i].name, queue[i].payload);
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
