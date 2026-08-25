// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_NOTE 1000

typedef struct {
    char name[MAX_NAME];
    int score;
    char note[MAX_NOTE];
} Student;

Student students[1000];
int stu_count = 0;

int find_student(const char *name) {
    for (int i = 0; i < stu_count; i++) {
        if (strcmp(students[i].name, name) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], name[MAX_NAME], note[MAX_NOTE];
    int score;
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %d", name, &score);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            
            int idx = find_student(name);
            if (idx == -1) {
                strcpy(students[stu_count].name, name);
                students[stu_count].score = score;
                strcpy(students[stu_count].note, note);
                stu_count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            scanf("%s %d", name, &score);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            
            int idx = find_student(name);
            if (idx != -1) {
                students[idx].score = score;
                strcpy(students[idx].note, note);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s", name);
            int idx = find_student(name);
            if (idx != -1) {
                for (int i = idx; i < stu_count - 1; i++) {
                    students[i] = students[i + 1];
                }
                stu_count--;
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (stu_count > 0) {
                int sum = 0;
                for (int i = 0; i < stu_count; i++) {
                    sum += students[i].score;
                }
                printf("%d\n", sum / stu_count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < stu_count; i++) {
                printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
            }
        }
    }
    
    return 0;
}