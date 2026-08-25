// F025.c
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
    char *note;
} Student;

typedef struct {
    char *name;
    Student *students;
    size_t student_count;
} Class;

int main(void) {
    Class *classes = NULL;
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
        if (strcmp(cmd, "CLASS") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && *name != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Class *new_classes = realloc(classes, capacity * sizeof(Class));
                    if (new_classes) classes = new_classes;
                }
                if (count < capacity) {
                    classes[count].name = strdup(name);
                    classes[count].students = NULL;
                    classes[count].student_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *cname = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *sname = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    classes[i].students = realloc(classes[i].students, (classes[i].student_count + 1) * sizeof(Student));
                    classes[i].students[classes[i].student_count].name = strdup(sname);
                    classes[i].students[classes[i].student_count].note = strdup(note);
                    classes[i].student_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *fromC = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *toC = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *sname = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long src_idx = -1, dest_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, fromC) == 0) src_idx = (long long)i;
                if (strcmp(classes[i].name, toC) == 0) dest_idx = (long long)i;
            }
            if (src_idx != -1 && dest_idx != -1 && src_idx != dest_idx) {
                for (size_t k = 0; k < classes[src_idx].student_count; k++) {
                    if (strcmp(classes[src_idx].students[k].name, sname) == 0) {
                        classes[dest_idx].students = realloc(classes[dest_idx].students, (classes[dest_idx].student_count + 1) * sizeof(Student));
                        classes[dest_idx].students[classes[dest_idx].student_count] = classes[src_idx].students[k];
                        classes[dest_idx].student_count++;
                        for (size_t j = k; j < classes[src_idx].student_count - 1; j++) {
                            classes[src_idx].students[j] = classes[src_idx].students[j + 1];
                        }
                        classes[src_idx].student_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *cname = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *sname = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    for (size_t k = 0; k < classes[i].student_count; k++) {
                        if (strcmp(classes[i].students[k].name, sname) == 0) {
                            free(classes[i].students[k].name);
                            free(classes[i].students[k].note);
                            for (size_t j = k; j < classes[i].student_count - 1; j++) {
                                classes[i].students[j] = classes[i].students[j + 1];
                            }
                            classes[i].student_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    free(classes[i].name);
                    for (size_t k = 0; k < classes[i].student_count; k++) {
                        free(classes[i].students[k].name);
                        free(classes[i].students[k].note);
                    }
                    free(classes[i].students);
                    for (size_t j = i; j < count - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("CLASS: %s\n", classes[i].name);
                for (size_t k = 0; k < classes[i].student_count; k++) {
                    printf("  %s - %s\n", classes[i].students[k].name, classes[i].students[k].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(classes[i].name);
        for (size_t k = 0; k < classes[i].student_count; k++) {
            free(classes[i].students[k].name);
            free(classes[i].students[k].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
