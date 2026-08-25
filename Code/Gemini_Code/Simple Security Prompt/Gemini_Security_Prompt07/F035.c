// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

int main() {
    Job *queue = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *name = strtok(NULL, " ");
            char *payload = strtok(NULL, "");
            if (name && payload) {
                queue = realloc(queue, (count + 1) * sizeof(Job));
                queue[count].name = strdup(name);
                queue[count].payload = strdup(payload);
                count++;
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *oldName = strtok(NULL, " ");
            char *newName = strtok(NULL, " ");
            if (oldName && newName) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, oldName) == 0) {
                        queue = realloc(queue, (count + 1) * sizeof(Job));
                        queue[count].name = strdup(newName);
                        queue[count].payload = strdup(queue[i].payload);
                        count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, name) == 0) {
                        free(queue[i].name);
                        free(queue[i].payload);
                        for (int j = i; j < count - 1; j++) {
                            queue[j] = queue[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(queue);
                    queue = NULL;
                } else {
                    queue = realloc(queue, count * sizeof(Job));
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, name) == 0) {
                        printf("%s\n", queue[i].payload);
                        free(queue[i].name);
                        free(queue[i].payload);
                        for (int j = i; j < count - 1; j++) {
                            queue[j] = queue[j + 1];
                        }
                        count--;
                        if (count == 0) {
                            free(queue);
                            queue = NULL;
                        } else {
                            queue = realloc(queue, count * sizeof(Job));
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
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
    return 0;
}
