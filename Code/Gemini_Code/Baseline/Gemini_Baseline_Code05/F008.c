// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

Student *gradebook = NULL;
int count = 0;
int capacity = 0;

int find_student(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(gradebook[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                char *p3 = strchr(p2 + 1, ' ');
                if (p3) {
                    *p3 = 0;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        gradebook = realloc(gradebook, capacity * sizeof(Student));
                    }
                    gradebook[count].name = strdup(p1);
                    gradebook[count].score = atoi(p2 + 1);
                    gradebook[count].note = strdup(p3 + 1);
                    count++;
                }
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *p1 = line + 7;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                char *p3 = strchr(p2 + 1, ' ');
                if (p3) {
                    *p3 = 0;
                    int idx = find_student(p1);
                    if (idx != -1) {
                        gradebook[idx].score = atoi(p2 + 1);
                        free(gradebook[idx].note);
                        gradebook[idx].note = strdup(p3 + 1);
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name = line + 5;
            int idx = find_student(name);
            if (idx != -1) {
                free(gradebook[idx].name);
                free(gradebook[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    gradebook[i] = gradebook[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long long sum = 0;
                for (int i = 0; i < count; i++) {
                    sum += gradebook[i].score;
                }
                printf("%lld\n", sum / count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", gradebook[i].name, gradebook[i].score, gradebook[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(gradebook[i].name);
        free(gradebook[i].note);
    }
    free(gradebook);
    return 0;
}
