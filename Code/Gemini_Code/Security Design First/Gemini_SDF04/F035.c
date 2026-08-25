// F035.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} Job;

Job *queue = NULL;
int count = 0;
int capacity = 0;

int find_job(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(queue[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "ENQUEUE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *payload = space2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Job *tmp = realloc(queue, capacity * sizeof(Job));
                        if (!tmp) {
                            free(line);
                            return 1;
                        }
                        queue = tmp;
                    }
                    queue[count].name = strdup(arg1);
                    queue[count].payload = strdup(payload);
                    if (queue[count].name && queue[count].payload) {
                        count++;
                    }
                }
            } else if (strcmp(cmd, "CLONE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *new_name = space2 + 1;
                    int idx = find_job(arg1);
                    if (idx != -1) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Job *tmp = realloc(queue, capacity * sizeof(Job));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            queue = tmp;
                        }
                        queue[count].name = strdup(new_name);
                        queue[count].payload = strdup(queue[idx].payload);
                        if (queue[count].name && queue[count].payload) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "CANCEL") == 0) {
                for (int i = 0; i < count; ) {
                    if (strcmp(queue[i].name, arg1) == 0) {
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
            } else if (strcmp(cmd, "RUN") == 0) {
                int idx = find_job(arg1);
                if (idx != -1) {
                    printf("%s\n", queue[idx].payload);
                    free(queue[idx].name);
                    free(queue[idx].payload);
                    for (int i = idx; i < count - 1; i++) {
                        queue[i] = queue[i + 1];
                    }
                    count--;
                }
            }
        } else {
            if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", queue[i].name);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].payload);
    }
    free(queue);
    return 0;
}