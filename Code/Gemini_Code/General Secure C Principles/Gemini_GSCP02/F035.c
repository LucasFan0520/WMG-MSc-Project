// F035.c
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
    size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ENQUEUE ", 8) == 0) {
            char name[256];
            char *p = line + 8;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) {
                char *payload = strchr(p, ' ');
                if (payload) {
                    while (*payload == ' ') payload++;
                }
                if (!payload) payload = "";
                Job *tmp = realloc(queue, (count + 1) * sizeof(Job));
                if (tmp) {
                    queue = tmp;
                    queue[count].name = strdup(name);
                    queue[count].payload = strdup(payload);
                    count++;
                }
            }
        } else if (strncmp(line, "CLONE ", 6) == 0) {
            char oldN[256], newN[256];
            if (sscanf(line + 6, "%255s %255s", oldN, newN) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, oldN) == 0) {
                        Job *tmp = realloc(queue, (count + 1) * sizeof(Job));
                        if (tmp) {
                            queue = tmp;
                            queue[count].name = strdup(newN);
                            queue[count].payload = strdup(queue[i].payload);
                            count++;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
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
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    printf("%s\n", queue[i].payload);
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", queue[i].name);
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
