// F025.c
#define _GNU_SOURCE
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
} RosterClass;

int main(void) {
    RosterClass *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name = line + 6;
            while (*name == ' ') name++;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    RosterClass *tmp = realloc(arr, capacity * sizeof(RosterClass));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].name = strdup(name);
                if (!arr[count].name) exit(1);
                arr[count].students = NULL;
                arr[count].student_count = 0;
                arr[count].student_capacity = 0;
                count++;
            }
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *cname = p;
            while (*p && *p != ' ') p++;
            char *sname = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                sname = p;
            }
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                note = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, cname) == 0) {
                    RosterClass *rc = &arr[i];
                    if (rc->student_count >= rc->student_capacity) {
                        rc->student_capacity = rc->student_capacity == 0 ? 4 : rc->student_capacity * 2;
                        Student *tmp = realloc(rc->students, rc->student_capacity * sizeof(Student));
                        if (!tmp) exit(1);
                        rc->students = tmp;
                    }
                    rc->students[rc->student_count].name = strdup(sname);
                    rc->students[rc->student_count].note = strdup(note);
                    if (!rc->students[rc->student_count].name || !rc->students[rc->student_count].note) exit(1);
                    rc->student_count++;
                    break;
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *fromClass = p;
            while (*p && *p != ' ') p++;
            char *toClass = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                toClass = p;
            }
            while (*p && *p != ' ') p++;
            char *sname = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                sname = p;
            }
            int idxFrom = -1, idxTo = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, fromClass) == 0) idxFrom = (int)i;
                if (strcmp(arr[i].name, toClass) == 0) idxTo = (int)i;
            }
            if (idxFrom != -1 && idxTo != -1) {
                RosterClass *f = &arr[idxFrom];
                RosterClass *t = &arr[idxTo];
                for (size_t j = 0; j < f->student_count; j++) {
                    if (strcmp(f->students[j].name, sname) == 0) {
                        if (t->student_count >= t->student_capacity) {
                            t->student_capacity = t->student_capacity == 0 ? 4 : t->student_capacity * 2;
                            Student *tmp = realloc(t->students, t->student_capacity * sizeof(Student));
                            if (!tmp) exit(1);
                            t->students = tmp;
                        }
                        t->students[t->student_count] = f->students[j]; // Wait, task_count? Typo check
                        // Let's use student_count!
                        t->students[t->student_count] = f->students[j];
                        t->student_count++;
                        for (size_t m = j; m < f->student_count - 1; m++) {
                            f->students[m] = f->students[m + 1];
                        }
                        f->student_count--;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *cname = p;
            while (*p && *p != ' ') p++;
            char *sname = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                sname = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, cname) == 0) {
                    RosterClass *rc = &arr[i];
                    for (size_t j = 0; j < rc->student_count; j++) {
                        if (strcmp(rc->students[j].name, sname) == 0) {
                            free(rc->students[j].name);
                            free(rc->students[j].note);
                            for (size_t m = j; m < rc->student_count - 1; m++) {
                                rc->students[m] = rc->students[m + 1];
                            }
                            rc->student_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *name = line + 12;
            while (*name == ' ') name++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name) == 0) {
                    free(arr[i].name);
                    for (size_t j = 0; j < arr[i].student_count; j++) {
                        free(arr[i].students[j].name);
                        free(arr[i].students[j].note);
                    }
                    free(arr[i].students);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Class: %s\n", arr[i].name);
                for (size_t j = 0; j < arr[i].student_count; j++) {
                    printf("  - %s: %s\n", arr[i].students[j].name, arr[i].students[j].note);
                }
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        for (size_t j = 0; j < arr[i].student_count; j++) {
            free(arr[i].students[j].name);
            free(arr[i].students[j].note);
        }
        free(arr[i].students);
    }
    free(arr);
    free(line);
    return 0;
}
