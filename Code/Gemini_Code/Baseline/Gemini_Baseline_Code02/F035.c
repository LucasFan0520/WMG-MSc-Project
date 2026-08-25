// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *payload;
} Job;

int main() {
    Job *queue = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", queue[i].name, queue[i].payload);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                queue = realloc(queue, sizeof(Job) * (count + 1));
                queue[count].name = strdup(args);
                queue[count].payload = strdup(p2 + 1);
                count++;
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *oldName = args;
            char *p2 = strchr(oldName, ' ');
            if (p2) {
                *p2 = '\0';
                char *newName = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, oldName) == 0) {
                        queue = realloc(queue, sizeof(Job) * (count + 1));
                        queue[count].name = strdup(newName);
                        queue[count].payload = strdup(queue[i].payload);
                        count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, args) == 0) {
                    free(queue[i].name); free(queue[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    if (count == 0) { free(queue); queue = NULL; }
                    else { queue = realloc(queue, sizeof(Job) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, args) == 0) {
                    printf("%s\n", queue[i].payload);
                    free(queue[i].name); free(queue[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    if (count == 0) { free(queue); queue = NULL; }
                    else { queue = realloc(queue, sizeof(Job) * count); }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(queue[i].name); free(queue[i].payload);
    }
    free(queue);
    return 0;
}
