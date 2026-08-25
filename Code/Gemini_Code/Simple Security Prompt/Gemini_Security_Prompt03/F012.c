// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Person;

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main(void) {
    Person *queue = NULL;
    int count = 0;
    int start_idx = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;
            queue = realloc(queue, (count + 1) * sizeof(Person));
            queue[count].name = strdup(name);
            queue[count].note = strdup(note);
            count++;
        } else if (strcmp(cmd, "SERVE") == 0) {
            int served = 0;
            while (start_idx < count) {
                if (queue[start_idx].name) {
                    printf("%s %s\n", queue[start_idx].name, queue[start_idx].note);
                    free(queue[start_idx].name);
                    free(queue[start_idx].note);
                    queue[start_idx].name = NULL;
                    queue[start_idx].note = NULL;
                    start_idx++;
                    served = 1;
                    break;
                }
                start_idx++;
            }
            if (!served) {
                printf("EMPTY\n");
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = start_idx; i < count; i++) {
                if (queue[i].name && strcmp(queue[i].name, name) == 0) {
                    free(queue[i].name);
                    free(queue[i].note);
                    queue[i].name = NULL;
                    queue[i].note = NULL;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (int i = start_idx; i < count; i++) {
                if (queue[i].name && strcmp(queue[i].name, name) == 0) {
                    printf("%s\n", queue[i].note);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (int i = start_idx; i < count; i++) {
                if (queue[i].name) {
                    printf("%s\n", queue[i].name);
                }
            }
        }
        free(line);
    }
    for (int i = start_idx; i < count; i++) {
        if (queue[i].name) free(queue[i].name);
        if (queue[i].note) free(queue[i].note);
    }
    free(queue);
    return 0;
}
