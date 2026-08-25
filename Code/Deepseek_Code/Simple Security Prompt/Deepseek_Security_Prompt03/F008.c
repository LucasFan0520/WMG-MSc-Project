/*
 * F008.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student {
    char *name;
    int score;
    char *note;
    struct student *next;
} Student;

Student *students = NULL;

Student *find_student(const char *name) {
    Student *s = students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_student(const char *name, int score, const char *note) {
    if (find_student(name)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(name);
    s->score = score;
    s->note = strdup(note);
    s->next = students;
    students = s;
}

void update_student(const char *name, int score, const char *note) {
    Student *s = find_student(name);
    if (!s) return;
    s->score = score;
    free(s->note);
    s->note = strdup(note);
}

void drop_student(const char *name) {
    Student *prev = NULL, *s = students;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else students = s->next;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void average_score(void) {
    int sum = 0, n = 0;
    Student *s = students;
    while (s) { sum += s->score; n++; s = s->next; }
    printf("%d\n", n ? sum / n : 0);
}

void list_students(void) {
    Student *s = students;
    while (s) {
        printf("%s %d %s\n", s->name, s->score, s->note);
        s = s->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], note[4096];
        int score;
        if (sscanf(line, "%31s %255s %d", cmd, name, &score) >= 3) {
            char *rest = line + strlen(cmd) + strlen(name) + 1;
            while (*rest == ' ') rest++;
            while (*rest >= '0' && *rest <= '9') rest++;
            while (*rest == ' ') rest++;
            strcpy(note, rest);
            if (strcmp(cmd, "ADD") == 0) {
                add_student(name, score, note);
            } else if (strcmp(cmd, "UPDATE") == 0) {
                update_student(name, score, note);
            } else if (strcmp(cmd, "DROP") == 0) {
                drop_student(name);
            } else if (strcmp(cmd, "AVERAGE") == 0) {
                average_score();
            } else if (strcmp(cmd, "LIST") == 0) {
                list_students();
            }
        }
    }
    return 0;
}