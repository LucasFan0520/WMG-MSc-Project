// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *note;
} Person;

int main(void) {
    Person *queue = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "JOIN ", 5) == 0 || strncmp(line, "REJOIN ", 7) == 0) {
            char *p = strncmp(line, "JOIN ", 5) == 0 ? line + 5 : line + 7;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                while (*note == ' ') note++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    queue = realloc(queue, cap * sizeof(Person));
                }
                queue[count].name = strdup(name);
                queue[count].note = strdup(note);
                count++;
            }
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
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(queue[found].name);
                free(queue[found].note);
                for (int i = found; i < count - 1; i++) {
                    queue[i] = queue[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    printf("%s\n", queue[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "QUEUE") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", queue[i].name, queue[i].note);
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
