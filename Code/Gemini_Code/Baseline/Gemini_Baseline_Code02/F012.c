// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *note;
} Person;

int main() {
    Person *queue = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "SERVE") == 0) {
                if (count > 0) {
                    printf("%s %s\n", queue[0].name, queue[0].note);
                    free(queue[0].name);
                    free(queue[0].note);
                    for (int j = 0; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    if (count == 0) { free(queue); queue = NULL; }
                    else { queue = realloc(queue, sizeof(Person) * count); }
                }
            } else if (strcmp(cmd, "QUEUE") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", queue[i].name, queue[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *note = p2 + 1;
                queue = realloc(queue, sizeof(Person) * (count + 1));
                queue[count].name = strdup(name);
                queue[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].note);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    if (count == 0) { free(queue); queue = NULL; }
                    else { queue = realloc(queue, sizeof(Person) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    printf("%s\n", queue[i].note);
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].note);
    }
    free(queue);
    return 0;
}
