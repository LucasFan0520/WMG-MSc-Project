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
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                classes = realloc(classes, (count + 1) * sizeof(Class));
                classes[count].name = strdup(name);
                classes[count].students = NULL;
                classes[count].student_count = 0;
                count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cname = strtok(NULL, " ");
            char *sname = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (cname && sname && note) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, cname) == 0) {
                        classes[i].students = realloc(classes[i].students, (classes[i].student_count + 1) * sizeof(Student));
                        classes[i].students[classes[i].student_count].name = strdup(sname);
                        classes[i].students[classes[i].student_count].note = strdup(note);
                        classes[i].student_count++;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *fromC = strtok(NULL, " ");
            char *toC = strtok(NULL, " ");
            char *sname = strtok(NULL, " ");
            if (fromC && toC && sname) {
                int idxF = -1, idxT = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, fromC) == 0) idxF = i;
                    if (strcmp(classes[i].name, toC) == 0) idxT = i;
                }
                if (idxF != -1 && idxT != -1) {
                    for (int j = 0; j < classes[idxF].student_count; j++) {
                        if (strcmp(classes[idxF].students[j].name, sname) == 0) {
                            classes[idxT].students = realloc(classes[idxT].students, (classes[idxT].student_count + 1) * sizeof(Student));
                            classes[idxT].students[classes[idxT].student_count].name = strdup(classes[idxF].students[j].name);
                            classes[idxT].students[classes[idxT].student_count].note = strdup(classes[idxF].students[j].note);
                            classes[idxT].student_count++;
                            free(classes[idxF].students[j].name);
                            free(classes[idxF].students[j].note);
                            for (int k = j; k < classes[idxF].student_count - 1; k++) {
                                classes[idxF].students[k] = classes[idxF].students[k + 1];
                            }
                            classes[idxF].student_count--;
                            if (classes[idxF].student_count == 0) {
                                free(classes[idxF].students);
                                classes[idxF].students = NULL;
                            } else {
                                classes[idxF].students = realloc(classes[idxF].students, classes[idxF].student_count * sizeof(Student));
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *cname = strtok(NULL, " ");
            char *sname = strtok(NULL, " ");
            if (cname && sname) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, cname) == 0) {
                        for (int j = 0; j < classes[i].student_count; j++) {
                            if (strcmp(classes[i].students[j].name, sname) == 0) {
                                free(classes[i].students[j].name);
                                free(classes[i].students[j].note);
                                for (int k = j; k < classes[i].student_count - 1; k++) {
                                    classes[i].students[k] = classes[i].students[k + 1];
                                }
                                classes[i].student_count--;
                                if (classes[i].student_count == 0) {
                                    free(classes[i].students);
                                    classes[i].students = NULL;
                                } else {
                                    classes[i].students = realloc(classes[i].students, classes[i].student_count * sizeof(Student));
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(classes[i].name, name) == 0) {
                        free(classes[i].name);
                        for (int j = 0; j < classes[i].student_count; j++) {
                            free(classes[i].students[j].name);
                            free(classes[i].students[j].note);
                        }
                        free(classes[i].students);
                        for (int j = i; j < count - 1; j++) {
                            classes[j] = classes[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(classes);
                    classes = NULL;
                } else {
                    classes = realloc(classes, count * sizeof(Class));
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("Class: %s\n", classes[i].name);
                for (int j = 0; j < classes[i].student_count; j++) {
                    printf("  Student: %s - %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].student_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
