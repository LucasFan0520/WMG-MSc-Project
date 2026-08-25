// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 1000
#define MAX_LEN 1000

typedef struct {
    char name[100];
    int score;
    char note[MAX_LEN];
} Student;

Student students[MAX_STUDENTS];
int count = 0;

int find_student(const char *name) {
    for (int i = 0; i < count; i++) if (strcmp(students[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], note[MAX_LEN];
    int score;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %d", name, &score);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (find_student(name) == -1 && count < MAX_STUDENTS) {
                strcpy(students[count].name, name);
                students[count].score = score;
                strcpy(students[count].note, note);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            scanf("%s %d", name, &score);
            fgets(note, MAX_LEN, stdin);
            note[strcspn(note, "\n")] = 0;
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            int idx = find_student(name);
            if (idx != -1) {
                students[idx].score = score;
                strcpy(students[idx].note, note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s", name);
            int idx = find_student(name);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) students[i] = students[i + 1];
                count--;
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (count > 0) {
                int sum = 0;
                for (int i = 0; i < count; i++) sum += students[i].score;
                printf("%d\n", sum / count);
            } else printf("0\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
        }
    }
    return 0;
}