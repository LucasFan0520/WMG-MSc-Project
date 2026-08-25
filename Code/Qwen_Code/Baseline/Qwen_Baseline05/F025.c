// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_NOTE 1000

typedef struct {
    char student[MAX_NAME];
    char note[MAX_NOTE];
} Student;

typedef struct {
    char name[MAX_NAME];
    Student students[1000];
    int stu_count;
} Class;

Class classes[1000];
int class_count = 0;

int find_class(const char *name) {
    for (int i = 0; i < class_count; i++) if (strcmp(classes[i].name, name) == 0) return i;
    return -1;
}

int find_student(Class *c, const char *name) {
    for (int i = 0; i < c->stu_count; i++) if (strcmp(c->students[i].student, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], name[MAX_NAME], student[MAX_NAME], note[MAX_NOTE], from[MAX_NAME], to[MAX_NAME];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "CLASS") == 0) {
            scanf("%s", name);
            strcpy(classes[class_count].name, name);
            classes[class_count].stu_count = 0;
            class_count++;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            scanf("%s %s", name, student);
            fgets(note, MAX_NOTE, stdin);
            if (note[0] == ' ') memmove(note, note + 1, strlen(note));
            if (note[strlen(note) - 1] == '\n') note[strlen(note) - 1] = '\0';
            int idx = find_class(name);
            if (idx != -1) {
                strcpy(classes[idx].students[classes[idx].stu_count].student, student);
                strcpy(classes[idx].students[classes[idx].stu_count].note, note);
                classes[idx].stu_count++;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            scanf("%s %s %s", from, to, student);
            int idx_from = find_class(from);
            int idx_to = find_class(to);
            if (idx_from != -1 && idx_to != -1) {
                int s_idx = find_student(&classes[idx_from], student);
                if (s_idx != -1) {
                    classes[idx_to].students[classes[idx_to].stu_count] = classes[idx_from].students[s_idx];
                    classes[idx_to].stu_count++;
                    for (int i = s_idx; i < classes[idx_from].stu_count - 1; i++) {
                        classes[idx_from].students[i] = classes[idx_from].students[i + 1];
                    }
                    classes[idx_from].stu_count--;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            scanf("%s %s", name, student);
            int idx = find_class(name);
            if (idx != -1) {
                int s_idx = find_student(&classes[idx], student);
                if (s_idx != -1) {
                    for (int i = s_idx; i < classes[idx].stu_count - 1; i++) {
                        classes[idx].students[i] = classes[idx].students[i + 1];
                    }
                    classes[idx].stu_count--;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            scanf("%s", name);
            int idx = find_class(name);
            if (idx != -1) {
                for (int i = idx; i < class_count - 1; i++) classes[i] = classes[i + 1];
                class_count--;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < class_count; i++) {
                printf("%s\n", classes[i].name);
                for (int j = 0; j < classes[i].stu_count; j++) {
                    printf("  %s %s\n", classes[i].students[j].student, classes[i].students[j].note);
                }
            }
        }
    }
    return 0;
}