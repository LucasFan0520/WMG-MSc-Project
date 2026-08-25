// F025.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *student_name;
    char *note;
} Student;

typedef struct {
    char *class_name;
    Student *students;
    size_t student_count;
    size_t student_capacity;
} ClassRoster;

int main(void) {
    ClassRoster *rosters = NULL;
    size_t class_count = 0;
    size_t class_capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "CLASS") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            if (class_count >= class_capacity) {
                size_t new_cap = class_capacity == 0 ? 4 : class_capacity * 2;
                ClassRoster *new_rost = realloc(rosters, new_cap * sizeof(ClassRoster));
                if (!new_rost) continue;
                rosters = new_rost;
                class_capacity = new_cap;
            }
            char *cn = strdup(name);
            if (cn) {
                rosters[class_count].class_name = cn;
                rosters[class_count].students = NULL;
                rosters[class_count].student_count = 0;
                rosters[class_count].student_capacity = 0;
                class_count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *class_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *student = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;

            if (*class_name == '\0' || *student == '\0') continue;

            for (size_t i = 0; i < class_count; i++) {
                if (strcmp(rosters[i].class_name, class_name) == 0) {
                    ClassRoster *cr = &rosters[i];
                    if (cr->student_count >= cr->student_capacity) {
                        size_t new_cap = cr->student_capacity == 0 ? 4 : cr->student_capacity * 2;
                        Student *new_studs = realloc(cr->students, new_cap * sizeof(Student));
                        if (!new_studs) break;
                        cr->students = new_studs;
                        cr->student_capacity = new_cap;
                    }
                    char *sn = strdup(student);
                    char *nt = strdup(note);
                    if (sn && nt) {
                        cr->students[cr->student_count].student_name = sn;
                        cr->students[cr->student_count].note = nt;
                        cr->student_count++;
                    } else {
                        free(sn);
                        free(nt);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *from = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *to = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *student = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*from == '\0' || *to == '\0' || *student == '\0') continue;

            int f_idx = -1, t_idx = -1;
            for (size_t i = 0; i < class_count; i++) {
                if (strcmp(rosters[i].class_name, from) == 0) f_idx = (int)i;
                if (strcmp(rosters[i].class_name, to) == 0) t_idx = (int)i;
            }
            if (f_idx != -1 && t_idx != -1 && f_idx != t_idx) {
                ClassRoster *cf = &rosters[f_idx];
                ClassRoster *ct = &rosters[t_idx];
                for (size_t j = 0; j < cf->student_count; j++) {
                    if (strcmp(cf->students[j].student_name, student) == 0) {
                        if (ct->student_count >= ct->student_capacity) {
                            size_t new_cap = ct->student_capacity == 0 ? 4 : ct->student_capacity * 2;
                            Student *new_s = realloc(ct->students, new_cap * sizeof(Student));
                            if (!new_s) break;
                            ct->students = new_s;
                            ct->student_capacity = new_cap;
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
            while (*p == ' ' || *p == '\t') p++;
            char *class_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *student = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*class_name == '\0' || *student == '\0') continue;

            for (size_t i = 0; i < class_count; i++) {
                if (strcmp(rosters[i].class_name, class_name) == 0) {
                    ClassRoster *cr = &rosters[i];
                    for (size_t j = 0; j < cr->student_count; j++) {
                        if (strcmp(cr->students[j].student_name, student) == 0) {
                            free(cr->students[j].student_name);
                            free(cr->students[j].note);
                            for (size_t k = j; k < cr->student_count - 1; k++) {
                                cr->students[k] = cr->students[k + 1];
                            }
                            cr->student_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *class_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*class_name == '\0') continue;

            for (size_t i = 0; i < class_count; i++) {
                if (strcmp(rosters[i].class_name, class_name) == 0) {
                    free(rosters[i].class_name);
                    for (size_t j = 0; j < rosters[i].student_count; j++) {
                        free(rosters[i].students[j].student_name);
                        free(rosters[i].students[j].note);
                    }
                    free(rosters[i].students);
                    for (size_t j = i; j < class_count - 1; j++) {
                        rosters[j] = rosters[j + 1];
                    }
                    class_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < class_count; i++) {
                printf("Class: %s\n", rosters[i].class_name);
                for (size_t j = 0; j < rosters[i].student_count; j++) {
                    printf("  Student: %s - %s\n", rosters[i].students[j].student_name, rosters[i].students[j].note);
                }
            }
        }
    }

    for (size_t i = 0; i < class_count; i++) {
        free(rosters[i].class_name);
        for (size_t j = 0; j < rosters[i].student_count; j++) {
            free(rosters[i].students[j].student_name);
            free(rosters[i].students[j].note);
        }
        free(rosters[i].students);
    }
    free(rosters);
    free(line);
    return 0;
}
