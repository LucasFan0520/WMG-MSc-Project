// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

int main() {
    Person *queue = NULL;
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
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char name[256];
            int n_len = 0;
            if (sscanf(ptr, "%255s%n", name, &n_len) > 0) {
                char *note = ptr + n_len;
                while (*note == ' ') note++;
                queue = realloc(queue, (count + 1) * sizeof(Person));
                queue[count].name = strdup(name);
                queue[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (count > 0) {
                printf("%s %s\n", queue[0].name, queue[0].note);
                free(queue[0].name);
                free(queue[0].note);
                for (int i = 0; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
                if (count > 0) {
                    queue = realloc(queue, count * sizeof(Person));
                } else {
                    free(queue);
                    queue = NULL;
                }
            } else {
                printf("QUEUE_EMPTY\n");
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(ptr, "%255s", name) > 0) {
                int i = 0;
                while (i < count) {
                    if (strcmp(queue[i].name, name) == 0) {
                        free(queue[i].name);
                        free(queue[i].note);
                        for (int j = i; j < count - 1; j++) {
                            queue[j] = queue[j + 1];
                        }
                        count--;
                    } else {
                        i++;
                    }
                }
                if (count > 0) {
                    queue = realloc(queue, count * sizeof(Person));
                } else {
                    free(queue);
                    queue = NULL;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
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
                    printf("%s\n", queue[found].note);
                } else {
                    printf("NOT_FOUND\n");
                }
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", queue[i].name, queue[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);
    return 0;
}
