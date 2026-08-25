// F025.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} ClassStudent;

typedef struct {
    char *name;
    ClassStudent *students;
    size_t student_count;
} Classroom;

int main(void) {
    Classroom *classes = NULL;
    size_t count = 0;
    char *line = NULL;
    size_t line_len = 0;
    ssize_t read;
    while ((read = getline(&line, &line_len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (read > 1 && line[read - 2] == '\r') {
            line[read - 2] = '\0';
        }
        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        if (*p == '\0') {
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') {
            p++;
        }
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "CLASS") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                Classroom *temp = realloc(classes, (count + 1) * sizeof(Classroom));
                if (temp) {
                    classes = temp;
                    classes[count].name = strdup(name);
                    classes[count].students = NULL;
                    classes[count].student_count = 0;
                    count++;
                }
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *cls = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *stud = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cls) == 0) {
                    ClassStudent *temp = realloc(classes[i].students, (classes[i].student_count + 1) * sizeof(ClassStudent));
                    if (temp) {
                        classes[i].students = temp;
                        classes[i].students[classes[i].student_count].name = strdup(stud);
                        classes[i].students[classes[i].student_count].note = strdup(note);
                        classes[i].student_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *fromClass = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *toClass = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *stud = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            int from_idx = -1, to_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, fromClass) == 0) from_idx = (int)i;
                if (strcmp(classes[i].name, toClass) == 0) to_idx = (int)i;
            }
            if (from_idx != -1 && to_idx != -1) {
                for (size_t k = 0; k < classes[from_idx].student_count; k++) {
                    if (strcmp(classes[from_idx].students[k].name, stud) == 0) {
                        ClassStudent *temp = realloc(classes[to_idx].students, (classes[to_idx].student_count + 1) * sizeof(ClassStudent));
                        if (temp) {
                            classes[to_idx].students = temp;
                            classes[to_idx].students[classes[to_idx].student_count] = classes[from_idx].students[k];
                            classes[to_idx].student_count++;
                            for (size_t j = k; j < classes[from_idx].student_count - 1; j++) {
                                classes[from_idx].students[j] = classes[from_idx].students[j + 1];
                            }
                            classes[from_idx].student_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *cls = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *stud = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cls) == 0) {
                    for (size_t k = 0; k < classes[i].student_count; k++) {
                        if (strcmp(classes[i].students[k].name, stud) == 0) {
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
            while (*p == ' ' || *p == '\t') {
                p++;
            }
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') {
                p++;
            }
            if (*p) {
                *p = '\0';
            }
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
                printf("Class: %s\n", classes[i].name);
                for (size_t k = 0; k < classes[i].student_count; k++) {
                    printf("  Student: %s - %s\n", classes[i].students[k].name, classes[i].students[k].note);
                }
            }
        }
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
    free(line);
    return 0;
}
