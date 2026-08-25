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
int count = 0;

int find_name(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char cmd[20];
    char name[1000];
    char note[10000];
    int score;

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            scanf("%s %d", name, &score);
            fgets(note, sizeof(note), stdin);
            if (note[0] == ' ') {
                memmove(note, note + 1, strlen(note));
            }
            note[strcspn(note, "\n")] = 0;
            int idx = find_name(name);
            if (idx != -1) {
                students[idx].score = score;
                free(students[idx].note);
                students[idx].note = strdup(note);
            } else {
                students = realloc(students, sizeof(Student) * (count + 1));
                students[count].name = strdup(name);
                students[count].score = score;
                students[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(students[idx].name);
                free(students[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    students[i] = students[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                int sum = 0;
                for (int i = 0; i < count; i++) {
                    sum += students[i].score;
                }
                printf("%d\n", sum / count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(students[i].name);
        free(students[i].note);
    }
    free(students);

    return 0;
}