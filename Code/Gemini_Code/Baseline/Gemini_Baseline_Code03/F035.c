// F035.c
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char name[256];
            int name_len = 0;
            if (sscanf(ptr, "%255s%n", name, &name_len) > 0) {
                char *payload = ptr + name_len;
                while (*payload == ' ') payload++;
                queue = realloc(queue, (count + 1) * sizeof(CopyJob));
                queue[count].name = strdup(name);
                queue[count].payload = strdup(payload);
                count++;
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char oldName[256], newName[256];
            if (sscanf(ptr, "%255s %255s", oldName, newName) == 2) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, oldName) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    queue = realloc(queue, (count + 1) * sizeof(CopyJob));
                    queue[count].name = strdup(newName);
                    queue[count].payload = strdup(queue[found].payload);
                    count++;
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
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
                    if (count > 0) {
                        queue = realloc(queue, count * sizeof(CopyJob));
                    } else {
                        free(queue);
                        queue = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    printf("%s\n", queue[found].payload);
                    free(queue[found].name);
                    free(queue[found].payload);
                    for (int i = found; i < count - 1; i++) {
                        queue[i] = queue[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        queue = realloc(queue, count * sizeof(CopyJob));
                    } else {
                        free(queue);
                        queue = NULL;
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
