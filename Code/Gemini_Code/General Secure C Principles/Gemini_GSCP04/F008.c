// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
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
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    long long score;
    char *note;
} Student;

int main(void) {
    Student *students = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *nm = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *sc_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *nt = p;
            long long sc = atoll(sc_s);
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(students[i].name, nm) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && *nm != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Student *new_students = realloc(students, capacity * sizeof(Student));
                    if (new_students) students = new_students;
                }
                if (count < capacity) {
                    students[count].name = strdup(nm);
                    students[count].score = sc;
                    students[count].note = strdup(nt);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *nm = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *sc_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *nt = p;
            long long sc = atoll(sc_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(students[i].name, nm) == 0) {
                    students[i].score = sc;
                    free(students[i].note);
                    students[i].note = strdup(nt);
                    break;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *nm = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(students[i].name, nm) == 0) {
                    free(students[i].name);
                    free(students[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        students[j] = students[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long long sum = 0;
                for (size_t i = 0; i < count; i++) {
                    sum += students[i].score;
                }
                printf("%lld\n", sum / (long long)count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %lld %s\n", students[i].name, students[i].score, students[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(students[i].name);
        free(students[i].note);
    }
    free(students);
    return 0;
}
