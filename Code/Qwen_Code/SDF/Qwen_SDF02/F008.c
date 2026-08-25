// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Student {
    char *name;
    int score;
    char *note;
    struct Student *next;
} Student;

Student *students = NULL;

Student* find_student(const char *name) {
    for (Student *s = students; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char name[256];
            int score;
            int n = 0;
            if (sscanf(rest, "%255s %d%n", name, &score, &n) != 2) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            Student *s = find_student(name);
            if (s) {
                s->score = score;
                free(s->note);
                s->note = strdup(note);
            } else {
                s = malloc(sizeof(Student));
                if (!s) continue;
                s->name = strdup(name);
                s->score = score;
                s->note = strdup(note);
                s->next = students;
                students = s;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Student *prev = NULL;
            for (Student *s = students; s; prev = s, s = s->next) {
                if (strcmp(s->name, name) == 0) {
                    if (prev) prev->next = s->next;
                    else students = s->next;
                    free(s->name);
                    free(s->note);
                    free(s);
                    break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            int sum = 0, count = 0;
            for (Student *s = students; s; s = s->next) {
                sum += s->score;
                count++;
            }
            if (count > 0) printf("%d\n", sum / count);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Student *s = students; s; s = s->next)
                printf("%s %d %s\n", s->name, s->score, s->note);
        }
    }
    while (students) {
        Student *s = students;
        students = students->next;
        free(s->name);
        free(s->note);
        free(s);
    }
    return 0;
}