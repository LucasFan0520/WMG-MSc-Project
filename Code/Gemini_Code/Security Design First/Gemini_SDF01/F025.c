// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Student;

typedef struct {
    char *name;
    Student *students;
    size_t student_count;
    size_t student_capacity;
} Classroom;

int main(void) {
    Classroom *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "CLASS") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found == -1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Classroom *new_items = realloc(items, capacity * sizeof(Classroom));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].name = strdup(name);
                if (!items[count].name) exit(1);
                items[count].students = NULL;
                items[count].student_count = 0;
                items[count].student_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *classname = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *student = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, classname) == 0) {
                    Classroom *cls = &items[i];
                    if (cls->student_count >= cls->student_capacity) {
                        cls->student_capacity = cls->student_capacity == 0 ? 4 : cls->student_capacity * 2;
                        Student *new_studs = realloc(cls->students, cls->student_capacity * sizeof(Student));
                        if (!new_studs) exit(1);
                        cls->students = new_studs;
                    }
                    cls->students[cls->student_count].name = strdup(student);
                    cls->students[cls->student_count].note = strdup(note);
                    if (!cls->students[cls->student_count].name || !cls->students[cls->student_count].note) exit(1);
                    cls->student_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *fromClass = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *toClass = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *student = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int idxFrom = -1, idxTo = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, fromClass) == 0) idxFrom = (int)i;
                if (strcmp(items[i].name, toClass) == 0) idxTo = (int)i;
            }

            if (idxFrom != -1 && idxTo != -1) {
                Classroom *cf = &items[idxFrom];
                Classroom *ct = &items[idxTo];
                for (size_t j = 0; j < cf->student_count; j++) {
                    if (strcmp(cf->students[j].name, student) == 0) {
                        if (ct->student_count >= ct->student_capacity) {
                            ct->student_capacity = ct->student_capacity == 0 ? 4 : ct->student_capacity * 2;
                            Student *new_studs = realloc(ct->students, ct->student_capacity * sizeof(Student));
                            if (!new_studs) exit(1);
                            ct->students = new_studs;
                        }
                        ct->students[ct->student_count] = cf->students[j];
                        ct->student_count++;

                        for (size_t k = j; k < cf->student_count - 1; k++) {
                            cf->students[k] = cf->students[k + 1];
                        }
                        cf->student_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *classname = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *student = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, classname) == 0) {
                    Classroom *cls = &items[i];
                    for (size_t j = 0; j < cls->student_count; j++) {
                        if (strcmp(cls->students[j].name, student) == 0) {
                            free(cls->students[j].name);
                            free(cls->students[j].note);
                            for (size_t k = j; k < cls->student_count - 1; k++) {
                                cls->students[k] = cls->students[k + 1];
                            }
                            cls->student_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *classname = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, classname) == 0) {
                    free(items[i].name);
                    for (size_t j = 0; j < items[i].student_count; j++) {
                        free(items[i].students[j].name);
                        free(items[i].students[j].note);
                    }
                    free(items[i].students);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Class: %s\n", items[i].name);
                for (size_t j = 0; j < items[i].student_count; j++) {
                    printf("  Student: %s - %s\n", items[i].students[j].name, items[i].students[j].note);
                }
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        for (size_t j = 0; j < items[i].student_count; j++) {
            free(items[i].students[j].name);
            free(items[i].students[j].note);
        }
        free(items[i].students);
    }
    free(items);
    free(line);
    return 0;
}
