// F012.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

int main(void) {
    Person *queue = NULL;
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
        if (strncmp(line, "JOIN ", 5) == 0 || strncmp(line, "REJOIN ", 7) == 0) {
            char *p = line + 5;
            if (strncmp(line, "REJOIN ", 7) == 0) p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            char *note = "";
            if (space) {
                *space = '\0';
                note = space + 1;
                while (*note == ' ') note++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Person *tmp = realloc(queue, new_cap * sizeof(Person));
                if (!tmp) return 1;
                queue = tmp;
                capacity = new_cap;
            }
            char *nn = strdup(name);
            char *nt = strdup(note);
            if (!nn || !nt) return 1;
            queue[count].name = nn;
            queue[count].note = nt;
            count++;
        } else if (strcmp(line, "SERVE") == 0) {
            if (count > 0) {
                printf("%s %s\n", queue[0].name, queue[0].note);
                free(queue[0].name);
                free(queue[0].note);
                for (int i = 0; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int w = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].note);
                } else {
                    queue[w++] = queue[i];
                }
            }
            count = w;
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) printf("%s\n", queue[found].note);
            else printf("NOT_FOUND\n");
        } else if (strcmp(line, "QUEUE") == 0) {
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
