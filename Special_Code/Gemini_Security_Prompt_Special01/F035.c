// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
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
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *payload = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                payload++;
            }
            while (*payload == ' ') payload++;
            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Job *new_q = realloc(queue, capacity * sizeof(Job));
                if (new_q) queue = new_q;
            }
            queue[count].name = mystrdup(name);
            queue[count].payload = mystrdup(payload);
            count++;
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *oldN = p;
            while (*oldN == ' ') oldN++;
            char *oldN_end = oldN;
            while (*oldN_end && *oldN_end != ' ') oldN_end++;
            char *newN = oldN_end;
            if (*oldN_end != '\0') {
                *oldN_end = '\0';
                newN++;
            }
            while (*newN == ' ') newN++;
            char *newN_end = newN;
            while (*newN_end && *newN_end != ' ') newN_end++;
            *newN_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(queue[i].name, oldN) == 0) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Job *new_q = realloc(queue, capacity * sizeof(Job));
                        if (new_q) queue = new_q;
                    }
                    queue[count].name = mystrdup(newN);
                    queue[count].payload = mystrdup(queue[i].payload);
                    count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
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
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
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
