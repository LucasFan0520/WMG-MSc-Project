// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *payload;
} Job;

int main(void) {
    Job *queue = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ENQUEUE ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *payload = p + 1;
                while (*payload == ' ') payload++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Job *new_q = realloc(queue, capacity * sizeof(Job));
                    if (new_q) queue = new_q;
                }
                if (count < capacity) {
                    queue[count].name = strdup(name);
                    queue[count].payload = strdup(payload);
                    count++;
                }
            }
        } else if (strncmp(line, "CLONE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *oldName = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *newName = p + 1;
                while (*newName == ' ') newName++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(queue[i].name, oldName) == 0) {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Job *new_q = realloc(queue, capacity * sizeof(Job));
                            if (new_q) queue = new_q;
                        }
                        if (count < capacity) {
                            queue[count].name = strdup(newName);
                            queue[count].payload = strdup(queue[i].payload);
                            count++;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "RUN ", 4) == 0) {
            char *name = line + 4;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(queue[i].name, name) == 0) {
                    printf("%s\n", queue[i].payload);
                    free(queue[i].name);
                    free(queue[i].payload);
                    for (int j = i; j < count - 1; j++) {
                        queue[j] = queue[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", queue[i].name);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(queue[i].name);
        free(queue[i].payload);
    }
    free(queue);
    return 0;
}
