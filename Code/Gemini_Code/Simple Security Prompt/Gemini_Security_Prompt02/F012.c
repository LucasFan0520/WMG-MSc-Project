// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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

typedef struct {
    char *name;
    char *note;
} Person;

int main() {
    Person *arr = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "JOIN ", 5) == 0 || strncmp(line, "REJOIN ", 7) == 0) {
            char *name_start = line + (line[0] == 'J' ? 5 : 7);
            char *space = strchr(name_start, ' ');
            char *note_start = "";
            if (space) {
                *space = '\0';
                note_start = space + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                arr = realloc(arr, cap * sizeof(Person));
            }
            arr[count].name = strdup(name_start);
            arr[count].note = strdup(note_start);
            count++;
        } else if (strcmp(line, "SERVE") == 0) {
            if (count > 0) {
                printf("%s %s\n", arr[0].name, arr[0].note);
                free(arr[0].name);
                free(arr[0].note);
                for (size_t i = 0; i < count - 1; i++) {
                    arr[i] = arr[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "CANCEL ", 7) == 0) {
            char *name_start = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name_start) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name_start = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name_start) == 0) {
                    printf("%s\n", arr[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "QUEUE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", arr[i].name, arr[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
