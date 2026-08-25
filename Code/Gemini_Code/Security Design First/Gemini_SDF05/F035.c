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
    int capacity = 0;
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
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            char *payload = "";
            if (space) {
                *space = '\0';
                payload = space + 1;
                while (*payload == ' ') payload++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Job *tmp = realloc(queue, new_cap * sizeof(Job));
                if (!tmp) return 1;
                queue = tmp;
                capacity = new_cap;
            }
            queue[count].name = strdup(name);
            queue[count].payload = strdup(payload);
            count++;
        } else if (strncmp(line, "CLONE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *oldName = p;
            char *space = strchr(oldName, ' ');
            if (!space) continue;
            *space = '\0';
            char *newName = space + 1;
            while (*newName == ' ') newName++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, oldName) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Job *tmp = realloc(queue, new_cap * sizeof(Job));
                    if (!tmp) return 1;
                    queue = tmp;
                    capacity = new_cap;
                }
                queue[count].name = strdup(newName);
                queue[count].payload = strdup(queue[found].payload);
                count++;
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(queue[found].name);
                free(queue[found].payload);
                for (int i = found; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "RUN ", 4) == 0) {
            char *name = line + 4;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                printf("Running Job %s: %s\n", queue[found].name, queue[found].payload);
                free(queue[found].name);
                free(queue[found].payload);
                for (int i = found; i < count - 1; i++) {
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
    free(line);
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].payload);
    }
    free(queue);
    return 0;
}
