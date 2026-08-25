// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

char *read_line(FILE *f) {
    size_t size = 32;
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Student *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "AVERAGE") == 0) {
                if (count == 0) {
                    printf("0\n");
                } else {
                    int sum = 0;
                    for (int i = 0; i < count; i++) {
                        sum += list[i].score;
                    }
                    printf("%d\n", sum / count);
                }
            } else if (strcmp(line, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %d %s\n", list[i].name, list[i].score, list[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    int score = atoi(rest2);
                    char *note = p3 + 1;
                    int found = -1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(list[i].name, name) == 0) {
                            found = i;
                            break;
                        }
                    }
                    if (strcmp(cmd, "ADD") == 0 && found == -1) {
                        list = realloc(list, (count + 1) * sizeof(Student));
                        list[count].name = strdup(name);
                        list[count].score = score;
                        list[count].note = strdup(note);
                        count++;
                    } else if (strcmp(cmd, "UPDATE") == 0 && found != -1) {
                        list[found].score = score;
                        free(list[found].note);
                        list[found].note = strdup(note);
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].name);
                free(list[found].note);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Student));
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].note);
    }
    free(list);
    return 0;
}