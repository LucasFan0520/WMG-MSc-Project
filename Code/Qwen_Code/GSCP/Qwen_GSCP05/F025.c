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
    int sc;
} Class;

Class *classes = NULL;
int cc = 0;

int find_class(const char *name) {
    for (int i = 0; i < cc; i++) {
        if (strcmp(classes[i].name, name) == 0) return i;
    }
    return -1;
}

int find_student(int ci, const char *name) {
    for (int j = 0; j < classes[ci].sc; j++) {
        if (strcmp(classes[ci].students[j].name, name) == 0) return j;
    }
    return -1;
}

int main(void) {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char a2[256];
        char a3[256];
        char rest[1024];
        a1[0] = '\0';
        a2[0] = '\0';
        a3[0] = '\0';
        rest[0] = '\0';
        int n = sscanf(line, "%31s %255s %255s %255s %1023[^\n]", cmd, a1, a2, a3, rest);
        if (n < 1) continue;

        if (strcmp(cmd, "CLASS") == 0 && n >= 2) {
            if (find_class(a1) < 0) {
                classes = realloc(classes, (cc + 1) * sizeof(Class));
                if (!classes) return 1;
                classes[cc].name = strdup(a1);
                classes[cc].students = NULL;
                classes[cc].sc = 0;
                cc++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0 && n >= 4) {
            int ci = find_class(a1);
            if (ci >= 0) {
                classes[ci].students = realloc(classes[ci].students, (classes[ci].sc + 1) * sizeof(Student));
                if (!classes[ci].students) return 1;
                classes[ci].students[classes[ci].sc].name = strdup(a2);
                classes[ci].students[classes[ci].sc].note = strdup(a3);
                classes[ci].sc++;
            }
        } else if (strcmp(cmd, "TRANSFER") == 0 && n >= 4) {
            int from = find_class(a1);
            int to = find_class(a2);
            if (from >= 0 && to >= 0) {
                int si = find_student(from, a3);
                if (si >= 0) {
                    classes[to].students = realloc(classes[to].students, (classes[to].sc + 1) * sizeof(Student));
                    if (!classes[to].students) return 1;
                    classes[to].students[classes[to].sc] = classes[from].students[si];
                    classes[to].sc++;
                    for (int k = si; k < classes[from].sc - 1; k++) {
                        classes[from].students[k] = classes[from].students[k + 1];
                    }
                    classes[from].sc--;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0 && n >= 3) {
            int ci = find_class(a1);
            if (ci >= 0) {
                int si = find_student(ci, a2);
                if (si >= 0) {
                    free(classes[ci].students[si].name);
                    free(classes[ci].students[si].note);
                    for (int k = si; k < classes[ci].sc - 1; k++) {
                        classes[ci].students[k] = classes[ci].students[k + 1];
                    }
                    classes[ci].sc--;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0 && n >= 2) {
            int ci = find_class(a1);
            if (ci >= 0) {
                for (int j = 0; j < classes[ci].sc; j++) {
                    free(classes[ci].students[j].name);
                    free(classes[ci].students[j].note);
                }
                free(classes[ci].students);
                free(classes[ci].name);
                for (int k = ci; k < cc - 1; k++) {
                    classes[k] = classes[k + 1];
                }
                cc--;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < cc; i++) {
                printf("%s\n", classes[i].name);
                for (int j = 0; j < classes[i].sc; j++) {
                    printf("  %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    for (int i = 0; i < cc; i++) {
        for (int j = 0; j < classes[i].sc; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
        free(classes[i].name);
    }
    free(classes);
    return 0;
}