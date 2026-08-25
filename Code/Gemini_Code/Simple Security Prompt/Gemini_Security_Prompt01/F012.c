// F012.c
#define _GNU_SOURCE
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

        if (strcmp(cmd, "JOIN") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            queue = realloc(queue, (count + 1) * sizeof(Person));
            if (!queue) return 1;
            queue[count].name = strdup(name);
            queue[count].note = strdup(note);
            count++;
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
                if (count > 0 && !queue) return 1;
            }
        } else if (strcmp(cmd, "CANCEL") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, args) == 0) {
                    free(queue[i].name);
                    free(queue[i].note);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    queue = realloc(queue, count * sizeof(Person));
                    if (count > 0 && !queue) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "REJOIN") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            char *note = "";
            if (space2) {
                *space2 = '\0';
                note = space2 + 1;
                while (*note == ' ') note++;
            }
            queue = realloc(queue, (count + 1) * sizeof(Person));
            if (!queue) return 1;
            queue[count].name = strdup(name);
            queue[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "FIND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, args) == 0) {
                    printf("%s\n", queue[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", queue[i].name, queue[i].note);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);
    return 0;
}
