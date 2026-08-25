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
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char name[256];
            int name_bytes = 0;
            if (sscanf(args, "%255s%n", name, &name_bytes) == 1) {
                char *note = args + name_bytes;
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
                queue = realloc(queue, count * sizeof(Person));
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, name) == 0) {
                        free(queue[i].name);
                        free(queue[i].note);
                        for (int j = i; j < count - 1; j++) {
                            queue[j] = queue[j + 1];
                        }
                        count--;
                        queue = realloc(queue, count * sizeof(Person));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, name) == 0) {
                        printf("%s\n", queue[i].note);
                        break;
                    }
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
