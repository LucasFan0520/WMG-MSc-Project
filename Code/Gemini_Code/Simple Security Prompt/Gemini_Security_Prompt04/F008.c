// F008.c
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
    int score;
    char *note;
} Student;

int main(void) {
    Student *students = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *endptr;
                long score = strtol(p + 1, &endptr, 10);
                p = endptr;
                while (*p == ' ') p++;
                char *note = p;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    students = realloc(students, cap * sizeof(Student));
                }
                students[count].name = strdup(name);
                students[count].score = score;
                students[count].note = strdup(note);
                count++;
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *endptr;
                long score = strtol(p + 1, &endptr, 10);
                p = endptr;
                while (*p == ' ') p++;
                char *note = p;
                for (int i = 0; i < count; i++) {
                    if (strcmp(students[i].name, name) == 0) {
                        students[i].score = score;
                        free(students[i].note);
                        students[i].note = strdup(note);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(students[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(students[found].name);
                free(students[found].note);
                for (int i = found; i < count - 1; i++) {
                    students[i] = students[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long sum = 0;
                for (int i = 0; i < count; i++) {
                    sum += students[i].score;
                }
                printf("%ld\n", sum / count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
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
