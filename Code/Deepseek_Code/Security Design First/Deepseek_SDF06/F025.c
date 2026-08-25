// F025.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    char *name;
    char *note;
} Student;

typedef struct Class {
    char *name;
    Student *students;
    size_t stu_count;
    size_t stu_cap;
} Class;

static Class *classes = NULL;
static size_t class_count = 0;
static size_t class_cap = 0;

static ssize_t find_class(const char *name) {
    for (size_t i = 0; i < class_count; i++)
        if (strcmp(classes[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static ssize_t find_student_in_class(Class *c, const char *name) {
    for (size_t i = 0; i < c->stu_count; i++)
        if (strcmp(c->students[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_student_to_class(Class *c, const char *name, const char *note) {
    if (find_student_in_class(c, name) != -1) return;
    if (c->stu_count == c->stu_cap) {
        c->stu_cap = c->stu_cap ? c->stu_cap * 2 : 4;
        Student *new = realloc(c->students, c->stu_cap * sizeof(Student));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        c->students = new;
    }
    c->students[c->stu_count].name = strdup(name);
    c->students[c->stu_count].note = strdup(note);
    if (!c->students[c->stu_count].name || !c->students[c->stu_count].note)
        { fprintf(stderr, "Memory error\n"); exit(1); }
    c->stu_count++;
}

static void remove_student_from_class(Class *c, size_t idx) {
    free(c->students[idx].name);
    free(c->students[idx].note);
    for (size_t i = idx; i < c->stu_count - 1; i++)
        c->students[i] = c->students[i + 1];
    c->stu_count--;
}

static void add_class(const char *name) {
    if (find_class(name) != -1) return;
    if (class_count == class_cap) {
        class_cap = class_cap ? class_cap * 2 : 4;
        Class *new = realloc(classes, class_cap * sizeof(Class));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        classes = new;
    }
    classes[class_count].name = strdup(name);
    if (!classes[class_count].name) { fprintf(stderr, "Memory error\n"); exit(1); }
    classes[class_count].students = NULL;
    classes[class_count].stu_count = 0;
    classes[class_count].stu_cap = 0;
    class_count++;
}

static void transfer_student(const char *from, const char *to, const char *name) {
    ssize_t fi = find_class(from);
    ssize_t ti = find_class(to);
    if (fi == -1 || ti == -1) return;
    ssize_t sidx = find_student_in_class(&classes[fi], name);
    if (sidx == -1) return;
    // add to destination
    add_student_to_class(&classes[ti], classes[fi].students[sidx].name,
                         classes[fi].students[sidx].note);
    // remove from source
    remove_student_from_class(&classes[fi], sidx);
}

static void drop_student(const char *classname, const char *name) {
    ssize_t ci = find_class(classname);
    if (ci == -1) return;
    ssize_t sidx = find_student_in_class(&classes[ci], name);
    if (sidx == -1) return;
    remove_student_from_class(&classes[ci], sidx);
}

static void delete_class(const char *name) {
    ssize_t idx = find_class(name);
    if (idx == -1) return;
    free(classes[idx].name);
    for (size_t i = 0; i < classes[idx].stu_count; i++) {
        free(classes[idx].students[i].name);
        free(classes[idx].students[i].note);
    }
    free(classes[idx].students);
    for (size_t i = idx; i < class_count - 1; i++)
        classes[i] = classes[i + 1];
    class_count--;
}

static void print_classes(void) {
    for (size_t i = 0; i < class_count; i++) {
        printf("%s", classes[i].name);
        for (size_t j = 0; j < classes[i].stu_count; j++)
            printf(" %s %s", classes[i].students[j].name, classes[i].students[j].note);
        putchar('\n');
    }
}

static void free_all(void) {
    for (size_t i = 0; i < class_count; i++) {
        free(classes[i].name);
        for (size_t j = 0; j < classes[i].stu_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], arg1[256], arg2[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "CLASS") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                add_class(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", name, arg1, arg2) == 3) {
                ssize_t idx = find_class(name);
                if (idx != -1)
                    add_student_to_class(&classes[idx], arg1, arg2);
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", name, arg1, arg2) == 3)
                transfer_student(name, arg1, arg2);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, arg1) == 2)
                drop_student(name, arg1);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_class(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }

    free(line);
    free_all();
    return 0;
}