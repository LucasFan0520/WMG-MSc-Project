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
    int student_count;
} Class;

int main() {
    Class *classes = NULL;
    int c_count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "CLASS") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                classes = realloc(classes, (c_count + 1) * sizeof(Class));
                classes[c_count].name = strdup(name);
                classes[c_count].students = NULL;
                classes[c_count].student_count = 0;
                c_count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char c_name[256], s_name[256];
            int bytes = 0;
            if (sscanf(args, "%255s %255s%n", c_name, s_name, &bytes) == 2) {
                char *note = args + bytes;
                while (*note == ' ') note++;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(classes[i].name, c_name) == 0) {
                        classes[i].students = realloc(classes[i].students, (classes[i].student_count + 1) * sizeof(Student));
                        classes[i].students[classes[i].student_count].name = strdup(s_name);
                        classes[i].students[classes[i].student_count].note = strdup(note);
                        classes[i].student_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256], to[256], student[256];
            if (sscanf(args, "%255s %255s %255s", from, to, student) == 3) {
                int idx_from = -1, idx_to = -1;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(classes[i].name, from) == 0) idx_from = i;
                    if (strcmp(classes[i].name, to) == 0) idx_to = i;
                }
                if (idx_from != -1 && idx_to != -1) {
                    for (int j = 0; j < classes[idx_from].student_count; j++) {
                        if (strcmp(classes[idx_from].students[j].name, student) == 0) {
                            classes[idx_to].students = realloc(classes[idx_to].students, (classes[idx_to].student_count + 1) * sizeof(Student));
                            classes[idx_to].students[classes[idx_to].student_count] = classes[idx_from].students[j];
                            classes[idx_to].student_count++;
                            for (int k = j; k < classes[idx_from].student_count - 1; k++) {
                                classes[idx_from].students[k] = classes[idx_from].students[k + 1];
                            }
                            classes[idx_from].student_count--;
                            classes[idx_from].students = realloc(classes[idx_from].students, classes[idx_from].student_count * sizeof(Student));
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char c_name[256], s_name[256];
            if (sscanf(args, "%255s %255s", c_name, s_name) == 2) {
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(classes[i].name, c_name) == 0) {
                        for (int j = 0; j < classes[i].student_count; j++) {
                            if (strcmp(classes[i].students[j].name, s_name) == 0) {
                                free(classes[i].students[j].name);
                                free(classes[i].students[j].note);
                                for (int k = j; k < classes[i].student_count - 1; k++) {
                                    classes[i].students[k] = classes[i].students[k + 1];
                                }
                                classes[i].student_count--;
                                classes[i].students = realloc(classes[i].students, classes[i].student_count * sizeof(Student));
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(classes[i].name, name) == 0) {
                        free(classes[i].name);
                        for (int j = 0; j < classes[i].student_count; j++) {
                            free(classes[i].students[j].name);
                            free(classes[i].students[j].note);
                        }
                        free(classes[i].students);
                        for (int j = i; j < c_count - 1; j++) {
                            classes[j] = classes[j + 1];
                        }
                        c_count--;
                        classes = realloc(classes, c_count * sizeof(Class));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("Class: %s\n", classes[i].name);
                for (int j = 0; j < classes[i].student_count; j++) {
                    printf("  Student: %s - %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    for (int i = 0; i < c_count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].student_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    classes = NULL;
    return 0;
}
