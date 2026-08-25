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
    int scount;
} Class;

Class *classes = NULL;
int ccount = 0;

int find_class(const char *name) {
    for (int i = 0; i < ccount; i++) {
        if (strcmp(classes[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "CLASS") == 0) {
            char name[100];
            scanf("%s", name);
            if (find_class(name) == -1) {
                classes = realloc(classes, sizeof(Class) * (ccount + 1));
                classes[ccount].name = strdup(name);
                classes[ccount].students = NULL;
                classes[ccount].scount = 0;
                ccount++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cls[100], stu[100], note[1000];
            scanf("%s %s", cls, stu);
            scanf(" %[^\n]", note);
            int idx = find_class(cls);
            if (idx != -1) {
                classes[idx].students = realloc(classes[idx].students, sizeof(Student) * (classes[idx].scount + 1));
                classes[idx].students[classes[idx].scount].name = strdup(stu);
                classes[idx].students[classes[idx].scount].note = strdup(note);
                classes[idx].scount++;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[100], to[100], stu[100];
            scanf("%s %s %s", from, to, stu);
            int fi = find_class(from);
            int ti = find_class(to);
            if (fi != -1 && ti != -1) {
                for (int i = 0; i < classes[fi].scount; i++) {
                    if (strcmp(classes[fi].students[i].name, stu) == 0) {
                        classes[ti].students = realloc(classes[ti].students, sizeof(Student) * (classes[ti].scount + 1));
                        classes[ti].students[classes[ti].scount] = classes[fi].students[i];
                        classes[ti].scount++;
                        for (int j = i; j < classes[fi].scount - 1; j++) {
                            classes[fi].students[j] = classes[fi].students[j+1];
                        }
                        classes[fi].scount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char cls[100], stu[100];
            scanf("%s %s", cls, stu);
            int idx = find_class(cls);
            if (idx != -1) {
                for (int i = 0; i < classes[idx].scount; i++) {
                    if (strcmp(classes[idx].students[i].name, stu) == 0) {
                        free(classes[idx].students[i].name);
                        free(classes[idx].students[i].note);
                        for (int j = i; j < classes[idx].scount - 1; j++) {
                            classes[idx].students[j] = classes[idx].students[j+1];
                        }
                        classes[idx].scount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[100];
            scanf("%s", name);
            int idx = find_class(name);
            if (idx != -1) {
                free(classes[idx].name);
                for (int i = 0; i < classes[idx].scount; i++) {
                    free(classes[idx].students[i].name);
                    free(classes[idx].students[i].note);
                }
                free(classes[idx].students);
                for (int i = idx; i < ccount - 1; i++) classes[i] = classes[i+1];
                ccount--;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < ccount; i++) {
                printf("%s\n", classes[i].name);
                for (int j = 0; j < classes[i].scount; j++) {
                    printf("  %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    for (int i = 0; i < ccount; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].scount; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}