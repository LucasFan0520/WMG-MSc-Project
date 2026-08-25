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
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256];
            int b = 0;
            if (sscanf(args, "%255s%n", name, &b) == 1) {
                char *payload = args + b;
                while (*payload == ' ') payload++;
                queue = realloc(queue, (count + 1) * sizeof(Job));
                queue[count].name = strdup(name);
                queue[count].payload = strdup(payload);
                count++;
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char oldName[256], newName[256];
            if (sscanf(args, "%255s %255s", oldName, newName) == 2) {
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
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, name) == 0) {
                        free(queue[i].name);
                        free(queue[i].payload);
                        for (int j = i; j < count - 1; j++) {
                            queue[j] = queue[j + 1];
                        }
                        count--;
                        queue = realloc(queue, count * sizeof(Job));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, name) == 0) {
                        printf("%s\n", queue[i].payload);
                        free(queue[i].name);
                        free(queue[i].payload);
                        for (int j = i; j < count - 1; j++) {
                            queue[j] = queue[j + 1];
                        }
                        count--;
                        queue = realloc(queue, count * sizeof(Job));
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
