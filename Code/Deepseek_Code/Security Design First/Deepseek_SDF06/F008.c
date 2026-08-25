// F008.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

static Student *students = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_student(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(students[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_student(const char *name, int score, const char *note) {
    ssize_t idx = find_student(name);
    if (idx != -1) {
        students[idx].score = score;
        free(students[idx].note);
        students[idx].note = strdup(note);
        if (!students[idx].note) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Student *new = realloc(students, capacity * sizeof(Student));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        students = new;
    }
    students[count].name = strdup(name);
    students[count].score = score;
    students[count].note = strdup(note);
    if (!students[count].name || !students[count].note) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void drop_student(const char *name) {
    ssize_t idx = find_student(name);
    if (idx == -1) return;
    free(students[idx].name);
    free(students[idx].note);
    for (size_t i = idx; i < count - 1; i++)
        students[i] = students[i + 1];
    count--;
}

static void average_score(void) {
    if (count == 0) { printf("0\n"); return; }
    int sum = 0;
    for (size_t i = 0; i < count; i++)
        sum += students[i].score;
    printf("%d\n", sum / (int)count);
}

static void list_students(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %d %s\n", students[i].name, students[i].score, students[i].note);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(students[i].name);
        free(students[i].note);
    }
    free(students);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], note[4096];
    int score;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %d %4095[^\n]", name, &score, note) == 3)
                add_student(name, score, note);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            if (sscanf(line, "%*s %255s %d %4095[^\n]", name, &score, note) == 3) {
                ssize_t idx = find_student(name);
                if (idx != -1)
                    add_student(name, score, note); // reuse add logic
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                drop_student(name);
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            average_score();
        } else if (strcmp(cmd, "LIST") == 0) {
            list_students();
        }
    }

    free(line);
    free_all();
    return 0;
}