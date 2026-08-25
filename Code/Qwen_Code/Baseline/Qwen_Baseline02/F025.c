// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Student;
typedef struct { char *name; Student *students; int nstudents; } Class;
Class classes[1000]; int nclasses = 0;

int find_class(char *name) {
    for (int i = 0; i < nclasses; i++) if (!strcmp(classes[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], student[100], note[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "CLASS")) {
            scanf("%s", name);
            classes[nclasses].name = strdup(name);
            classes[nclasses].students = NULL;
            classes[nclasses++].nstudents = 0;
        } else if (!strcmp(cmd, "STUDENT")) {
            scanf("%s", name);
            fgets(student, sizeof(student), stdin);
            student[strcspn(student, "\n")] = 0;
            if (student[0] == ' ') memmove(student, student + 1, strlen(student));
            char *space = strchr(student, ' ');
            if (space) {
                *space = 0;
                char *n = space + 1;
                while(*n == ' ') n++;
                int idx = find_class(name);
                if (idx != -1) {
                    classes[idx].students = realloc(classes[idx].students, sizeof(Student) * (classes[idx].nstudents + 1));
                    classes[idx].students[classes[idx].nstudents].name = strdup(student);
                    classes[idx].students[classes[idx].nstudents++].note = strdup(n);
                }
            }
        } else if (!strcmp(cmd, "TRANSFER")) {
            char from[100], to[100];
            scanf("%s %s", from, to);
            fgets(student, sizeof(student), stdin);
            student[strcspn(student, "\n")] = 0;
            if (student[0] == ' ') memmove(student, student + 1, strlen(student));
            int ifrom = find_class(from), ito = find_class(to);
            if (ifrom != -1 && ito != -1) {
                for (int i = 0; i < classes[ifrom].nstudents; i++) {
                    if (!strcmp(classes[ifrom].students[i].name, student)) {
                        classes[ito].students = realloc(classes[ito].students, sizeof(Student) * (classes[ito].nstudents + 1));
                        classes[ito].students[classes[ito].nstudents++] = classes[ifrom].students[i];
                        for (int j = i; j < classes[ifrom].nstudents - 1; j++) classes[ifrom].students[j] = classes[ifrom].students[j + 1];
                        classes[ifrom].nstudents--;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DROP")) {
            scanf("%s", name);
            fgets(student, sizeof(student), stdin);
            student[strcspn(student, "\n")] = 0;
            if (student[0] == ' ') memmove(student, student + 1, strlen(student));
            int idx = find_class(name);
            if (idx != -1) {
                for (int i = 0; i < classes[idx].nstudents; i++) {
                    if (!strcmp(classes[idx].students[i].name, student)) {
                        free(classes[idx].students[i].name);
                        free(classes[idx].students[i].note);
                        for (int j = i; j < classes[idx].nstudents - 1; j++) classes[idx].students[j] = classes[idx].students[j + 1];
                        classes[idx].nstudents--;
                        break;
                    }
                }
            }
        } else if (!strcmp(cmd, "DELETECLASS")) {
            scanf("%s", name);
            int idx = find_class(name);
            if (idx != -1) {
                free(classes[idx].name);
                for (int i = 0; i < classes[idx].nstudents; i++) {
                    free(classes[idx].students[i].name);
                    free(classes[idx].students[i].note);
                }
                free(classes[idx].students);
                for (int i = idx; i < nclasses - 1; i++) classes[i] = classes[i + 1];
                nclasses--;
            }
        } else if (!strcmp(cmd, "PRINT")) {
            for (int i = 0; i < nclasses; i++) {
                printf("%s\n", classes[i].name);
                for (int j = 0; j < classes[i].nstudents; j++) {
                    printf("  %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    for (int i = 0; i < nclasses; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].nstudents; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    return 0;
}