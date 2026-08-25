// F035.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

int main(void) {
    Job *queue = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ENQUEUE ", 8) == 0) {
            char *p = line + 8;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *payload = space + 1;
                Job *tmp = realloc(queue, sizeof(Job) * (count + 1));
                if (tmp) {
                    queue = tmp;
                    queue[count].name = strdup(name);
                    queue[count].payload = strdup(payload);
                    if (queue[count].name && queue[count].payload) {
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "CLONE ", 6) == 0) {
            char *p = line + 6;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *oldName = p;
                char *newName = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, oldName) == 0) {
                        Job *tmp = realloc(queue, sizeof(Job) * (count + 1));
                        if (tmp) {
                            queue = tmp;
                            queue[count].name = strdup(newName);
                            queue[count].payload = strdup(queue[i].payload);
                            if (queue[count].name && queue[count].payload) {
                                count++;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "RUN ", 4) == 0) {
            char *name = line + 4;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                printf("%s\n", queue[idx].payload);
                free(queue[idx].name);
                free(queue[idx].payload);
                for (int i = idx; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", queue[i].name);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].payload);
    }
    free(queue);
    free(line);
    return 0;
}
