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
    int count;
    int cap;
} Class;

Class *classes = NULL;
int c_count = 0;

int find_class(const char *name) {
    for (int i = 0; i < c_count; i++) {
        if (strcmp(classes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int find_student(int ci, const char *name) {
    for (int i = 0; i < classes[ci].count; i++) {
        if (strcmp(classes[ci].students[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[32];
    char name[1024];
    char cls[1024];
    char from_cls[1024];
    char to_cls[1024];
    char student[1024];
    char note[10240];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "CLASS") == 0) {
            scanf("%s", name);
            if (find_class(name) == -1) {
                classes = realloc(classes, sizeof(Class) * (c_count + 1));
                classes[c_count].name = strdup(name);
                classes[c_count].students = NULL;
                classes[c_count].count = 0;
                classes[c_count].cap = 0;
                c_count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            scanf("%s %s", cls, student);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = '\0';
            int ci = find_class(cls);
            if (ci != -1) {
                if (classes[ci].count == classes[ci].cap) {
                    classes[ci].cap = classes[ci].cap ? classes[ci].cap * 2 : 4;
                    classes[ci].students = realloc(classes[ci].students, sizeof(Student) * classes[ci].cap);
                }
                classes[ci].students[classes[ci].count].name = strdup(student);
                classes[ci].students[classes[ci].count].note = strdup(note);
                classes[ci].count++;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            scanf("%s %s %s", from_cls, to_cls, student);
            int fi = find_class(from_cls);
            int ti = find_class(to_cls);
            if (fi != -1 && ti != -1) {
                int si = find_student(fi, student);
                if (si != -1) {
                    if (classes[ti].count == classes[ti].cap) {
                        classes[ti].cap = classes[ti].cap ? classes[ti].cap * 2 : 4;
                        classes[ti].students = realloc(classes[ti].students, sizeof(Student) * classes[ti].cap);
                    }
                    classes[ti].students[classes[ti].count] = classes[fi].students[si];
                    classes[ti].count++;
                    for (int i = si; i < classes[fi].count - 1; i++) {
                        classes[fi].students[i] = classes[fi].students[i + 1];
                    }
                    classes[fi].count--;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s %s", cls, student);
            int ci = find_class(cls);
            if (ci != -1) {
                int si = find_student(ci, student);
                if (si != -1) {
                    free(classes[ci].students[si].name);
                    free(classes[ci].students[si].note);
                    for (int i = si; i < classes[ci].count - 1; i++) {
                        classes[ci].students[i] = classes[ci].students[i + 1];
                    }
                    classes[ci].count--;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            scanf("%s", cls);
            int ci = find_class(cls);
            if (ci != -1) {
                free(classes[ci].name);
                for (int i = 0; i < classes[ci].count; i++) {
                    free(classes[ci].students[i].name);
                    free(classes[ci].students[i].note);
                }
                free(classes[ci].students);
                for (int i = ci; i < c_count - 1; i++) {
                    classes[i] = classes[i + 1];
                }
                c_count--;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("%s\n", classes[i].name);
                for (int j = 0; j < classes[i].count; j++) {
                    printf("  %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }

    for (int i = 0; i < c_count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);

    return 0;
}