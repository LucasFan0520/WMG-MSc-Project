// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
} Class;

typedef struct {
    char *class_name;
    char *student_name;
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
    Class *classes = NULL;
    int c_count = 0;
    Student *students = NULL;
    int s_count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "PRINT") == 0) {
                for (int i = 0; i < c_count; i++) {
                    printf("Class: %s\n", classes[i].name);
                    for (int j = 0; j < s_count; j++) {
                        if (strcmp(students[j].class_name, classes[i].name) == 0) {
                            printf("  %s: %s\n", students[j].student_name, students[j].note);
                        }
                    }
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "CLASS") == 0) {
            int found = -1;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, rest) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                classes = realloc(classes, (c_count + 1) * sizeof(Class));
                classes[c_count].name = strdup(rest);
                c_count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *cname = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *sname = rest2;
                    char *note = p3 + 1;
                    int c_found = -1;
                    for (int i = 0; i < c_count; i++) {
                        if (strcmp(classes[i].name, cname) == 0) {
                            c_found = i;
                            break;
                        }
                    }
                    if (c_found != -1) {
                        students = realloc(students, (s_count + 1) * sizeof(Student));
                        students[s_count].class_name = strdup(cname);
                        students[s_count].student_name = strdup(sname);
                        students[s_count].note = strdup(note);
                        s_count++;
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *fromC = rest;
                char *rest2 = p2 + 1;
                char *p3 = strchr(rest2, ' ');
                if (p3) {
                    *p3 = '\0';
                    char *toC = rest2;
                    char *student = p3 + 1;
                    int to_found = -1;
                    for (int i = 0; i < c_count; i++) {
                        if (strcmp(classes[i].name, toC) == 0) {
                            to_found = i;
                            break;
                        }
                    }
                    if (to_found != -1) {
                        for (int i = 0; i < s_count; i++) {
                            if (strcmp(students[i].class_name, fromC) == 0 && strcmp(students[i].student_name, student) == 0) {
                                free(students[i].class_name);
                                students[i].class_name = strdup(toC);
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *cname = rest;
                char *sname = p2 + 1;
                int found = -1;
                for (int i = 0; i < s_count; i++) {
                    if (strcmp(students[i].class_name, cname) == 0 && strcmp(students[i].student_name, sname) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(students[found].class_name);
                    free(students[found].student_name);
                    free(students[found].note);
                    for (int i = found; i < s_count - 1; i++) {
                        students[i] = students[i + 1];
                    }
                    s_count--;
                    if (s_count == 0) {
                        free(students);
                        students = NULL;
                    } else {
                        students = realloc(students, s_count * sizeof(Student));
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *cname = rest;
            int c_found = -1;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    c_found = i;
                    break;
                }
            }
            if (c_found != -1) {
                free(classes[c_found].name);
                for (int i = c_found; i < c_count - 1; i++) {
                    classes[i] = classes[i + 1];
                }
                c_count--;
                if (c_count == 0) {
                    free(classes);
                    classes = NULL;
                } else {
                    classes = realloc(classes, c_count * sizeof(Class));
                }
                int idx = 0;
                while (idx < s_count) {
                    if (strcmp(students[idx].class_name, cname) == 0) {
                        free(students[idx].class_name);
                        free(students[idx].student_name);
                        free(students[idx].note);
                        for (int j = idx; j < s_count - 1; j++) {
                            students[j] = students[j + 1];
                        }
                        s_count--;
                    } else {
                        idx++;
                    }
                }
                if (s_count == 0) {
                    free(students);
                    students = NULL;
                } else {
                    students = realloc(students, s_count * sizeof(Student));
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < c_count; i++) {
        free(classes[i].name);
    }
    free(classes);
    for (int i = 0; i < s_count; i++) {
        free(students[i].class_name);
        free(students[i].student_name);
        free(students[i].note);
    }
    free(students);
    return 0;
}