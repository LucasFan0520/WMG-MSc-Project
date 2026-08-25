// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; int score; char *note; } Student;
Student *students = NULL;
int stu_count = 0, stu_cap = 0;

int find_student(const char *name) {
    for (int i = 0; i < stu_count; i++)
        if (strcmp(students[i].name, name) == 0) return i;
    return -1;
}

void add_student(const char *name, int score, const char *note) {
    int idx = find_student(name);
    if (idx >= 0) {
        students[idx].score = score;
        free(students[idx].note);
        students[idx].note = strdup(note);
        return;
    }
    if (stu_count == stu_cap) {
        stu_cap = stu_cap ? stu_cap * 2 : 4;
        students = realloc(students, sizeof(Student) * stu_cap);
    }
    students[stu_count].name = strdup(name);
    students[stu_count].score = score;
    students[stu_count].note = strdup(note);
    stu_count++;
}

void update_student(const char *name, int score, const char *note) {
    int idx = find_student(name);
    if (idx < 0) return;
    students[idx].score = score;
    free(students[idx].note);
    students[idx].note = strdup(note);
}

void drop_student(const char *name) {
    int idx = find_student(name);
    if (idx < 0) return;
    free(students[idx].name);
    free(students[idx].note);
    for (int i = idx; i < stu_count - 1; i++) students[i] = students[i + 1];
    stu_count--;
}

void average_score(void) {
    if (stu_count == 0) { printf("0\n"); return; }
    int sum = 0;
    for (int i = 0; i < stu_count; i++) sum += students[i].score;
    printf("%d\n", sum / stu_count);
}

void list_students(void) {
    for (int i = 0; i < stu_count; i++)
        printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], note[4096];
        int score;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %d %4095[^\n]", name, &score, note) == 3) {
                if (strcmp(cmd, "ADD") == 0) add_student(name, score, note);
                else update_student(name, score, note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) average_score();
        else if (strcmp(cmd, "LIST") == 0) list_students();
    }
    for (int i = 0; i < stu_count; i++) { free(students[i].name); free(students[i].note); }
    free(students);
    return 0;
}