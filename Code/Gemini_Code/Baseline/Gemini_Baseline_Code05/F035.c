// F035.c
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ENQUEUE ", 8) == 0) {
            char *p1 = line + 8;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    queue = realloc(queue, capacity * sizeof(Job));
                }
                queue[count].name = strdup(p1);
                queue[count].payload = strdup(sp + 1);
                count++;
            }
        } else if (strncmp(line, "CLONE ", 6) == 0) {
            char *p1 = line + 6;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                char *new_name = sp + 1;
                int idx = find_job(p1);
                if (idx != -1) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        queue = realloc(queue, capacity * sizeof(Job));
                    }
                    queue[count].name = strdup(new_name);
                    queue[count].payload = strdup(queue[idx].payload);
                    count++;
                }
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            int idx = find_job(name);
            if (idx != -1) {
                free(queue[idx].name);
                free(queue[idx].payload);
                for (int i = idx; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "RUN ", 4) == 0) {
            char *name = line + 4;
            int idx = find_job(name);
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
    return 0;
}
