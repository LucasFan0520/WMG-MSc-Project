// F008.c
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
    int score;
    char *note;
} Stud;

int main() {
    Stud *students = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "AVERAGE") == 0) {
                if (count == 0) {
                    printf("0\n");
                } else {
                    int sum = 0;
                    for (int i = 0; i < count; i++) sum += students[i].score;
                    printf("%d\n", sum / count);
                }
            } else if (strcmp(cmd, "LIST") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "ADD") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *score_str = p2 + 1;
                char *p3 = strchr(score_str, ' ');
                if (p3) {
                    *p3 = '\0';
                    int score = atoi(score_str);
                    char *note = p3 + 1;
                    students = realloc(students, sizeof(Stud) * (count + 1));
                    students[count].name = strdup(name);
                    students[count].score = score;
                    students[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *score_str = p2 + 1;
                char *p3 = strchr(score_str, ' ');
                if (p3) {
                    *p3 = '\0';
                    int score = atoi(score_str);
                    char *note = p3 + 1;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(students[i].name, name) == 0) {
                            students[i].score = score;
                            free(students[i].note);
                            students[i].note = strdup(note);
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(students[i].name, name) == 0) {
                    free(students[i].name);
                    free(students[i].note);
                    for (int j = i; j < count - 1; j++) {
                        students[j] = students[j + 1];
                    }
                    count--;
                    if (count == 0) { free(students); students = NULL; }
                    else { students = realloc(students, sizeof(Stud) * count); }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(students[i].name);
        free(students[i].note);
    }
    free(students);
    return 0;
}
