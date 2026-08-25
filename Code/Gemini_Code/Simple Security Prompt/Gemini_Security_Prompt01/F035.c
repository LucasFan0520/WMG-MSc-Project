// F035.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *payload;
} CopyJob;

int main() {
    CopyJob *queue = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ENQUEUE") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            char *payload = "";
            if (space2) {
                *space2 = '\0';
                payload = space2 + 1;
                while (*payload == ' ') payload++;
            }
            queue = realloc(queue, (count + 1) * sizeof(CopyJob));
            if (!queue) return 1;
            queue[count].name = strdup(name);
            queue[count].payload = strdup(payload);
            count++;
        } else if (strcmp(cmd, "CLONE") == 0 && args) {
            char *oldN = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *newN = space2 + 1;
                while (*newN == ' ') newN++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, oldN) == 0) {
                        queue = realloc(queue, (count + 1) * sizeof(CopyJob));
                        if (!queue) return 1;
                        queue[count].name = strdup(newN);
                        queue[count].payload = strdup(queue[i].payload);
                        count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, args) == 0) {
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    queue = realloc(queue, count * sizeof(CopyJob));
                    if (count > 0 && !queue) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "RUN") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, args) == 0) {
                    printf("%s %s\n", queue[i].name, queue[i].payload);
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    queue = realloc(queue, count * sizeof(CopyJob));
                    if (count > 0 && !queue) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
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
