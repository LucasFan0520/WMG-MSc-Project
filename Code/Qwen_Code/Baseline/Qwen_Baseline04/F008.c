// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

Student *students = NULL;
int stu_count = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char name[100], note[1000];
            int score;
            scanf("%s %d", name, &score);
            scanf(" %[^\n]", note);
            students = realloc(students, sizeof(Student) * (stu_count + 1));
            students[stu_count].name = strdup(name);
            students[stu_count].score = score;
            students[stu_count].note = strdup(note);
            stu_count++;
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char name[100], note[1000];
            int score;
            scanf("%s %d", name, &score);
            scanf(" %[^\n]", note);
            for (int i = 0; i < stu_count; i++) {
                if (strcmp(students[i].name, name) == 0) {
                    students[i].score = score;
                    free(students[i].note);
                    students[i].note = strdup(note);
                    break;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[100];
            scanf("%s", name);
            for (int i = 0; i < stu_count; i++) {
                if (strcmp(students[i].name, name) == 0) {
                    free(students[i].name);
                    free(students[i].note);
                    for (int j = i; j < stu_count - 1; j++) students[j] = students[j+1];
                    stu_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (stu_count == 0) {
                printf("0\n");
            } else {
                int sum = 0;
                for (int i = 0; i < stu_count; i++) sum += students[i].score;
                printf("%d\n", sum / stu_count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < stu_count; i++) {
                printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
            }
        }
    }
    for (int i = 0; i < stu_count; i++) {
        free(students[i].name);
        free(students[i].note);
    }
    free(students);
    return 0;
}