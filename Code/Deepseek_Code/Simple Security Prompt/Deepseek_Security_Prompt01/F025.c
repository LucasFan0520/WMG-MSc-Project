// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *student; char *note; } Student;
typedef struct { char *name; Student *students; int sc, scap; } Class;
Class *classes = NULL;
int cc = 0, ccap = 0;

int find_class(const char *name) {
    for (int i = 0; i < cc; i++)
        if (strcmp(classes[i].name, name) == 0) return i;
    return -1;
}

int find_student_in_class(Class *c, const char *student) {
    for (int i = 0; i < c->sc; i++)
        if (strcmp(c->students[i].student, student) == 0) return i;
    return -1;
}

void add_class(const char *name) {
    if (find_class(name) >= 0) return;
    if (cc == ccap) { ccap = ccap ? ccap * 2 : 4; classes = realloc(classes, sizeof(Class) * ccap); }
    classes[cc].name = strdup(name);
    classes[cc].students = NULL;
    classes[cc].sc = 0;
    classes[cc].scap = 0;
    cc++;
}

void add_student(const char *classname, const char *student, const char *note) {
    int idx = find_class(classname);
    if (idx < 0) return;
    Class *c = &classes[idx];
    if (find_student_in_class(c, student) >= 0) return;
    if (c->sc == c->scap) { c->scap = c->scap ? c->scap * 2 : 4; c->students = realloc(c->students, sizeof(Student) * c->scap); }
    c->students[c->sc].student = strdup(student);
    c->students[c->sc].note = strdup(note);
    c->sc++;
}

void transfer_student(const char *from, const char *to, const char *student) {
    int fi = find_class(from), ti = find_class(to);
    if (fi < 0 || ti < 0) return;
    Class *fc = &classes[fi];
    int si = find_student_in_class(fc, student);
    if (si < 0) return;
    add_student(to, student, fc->students[si].note);
    free(fc->students[si].student);
    free(fc->students[si].note);
    for (int i = si; i < fc->sc - 1; i++) fc->students[i] = fc->students[i + 1];
    fc->sc--;
}

void drop_student(const char *classname, const char *student) {
    int idx = find_class(classname);
    if (idx < 0) return;
    Class *c = &classes[idx];
    int si = find_student_in_class(c, student);
    if (si < 0) return;
    free(c->students[si].student);
    free(c->students[si].note);
    for (int i = si; i < c->sc - 1; i++) c->students[i] = c->students[i + 1];
    c->sc--;
}

void delete_class(const char *name) {
    int idx = find_class(name);
    if (idx < 0) return;
    for (int i = 0; i < classes[idx].sc; i++) {
        free(classes[idx].students[i].student);
        free(classes[idx].students[i].note);
    }
    free(classes[idx].students);
    free(classes[idx].name);
    for (int i = idx; i < cc - 1; i++) classes[i] = classes[i + 1];
    cc--;
}

void print_classes(void) {
    for (int i = 0; i < cc; i++) {
        printf("%s\n", classes[i].name);
        for (int j = 0; j < classes[i].sc; j++)
            printf("%s %s\n", classes[i].students[j].student, classes[i].students[j].note);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) add_class(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            if (sscanf(line, "%*s %255s %255s %4095[^\n]", arg1, arg2, arg3) == 3)
                add_student(arg1, arg2, arg3);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", arg1, arg2, arg3) == 3)
                transfer_student(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                drop_student(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) delete_class(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) print_classes();
    }
    for (int i = 0; i < cc; i++) {
        for (int j = 0; j < classes[i].sc; j++) {
            free(classes[i].students[j].student);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
        free(classes[i].name);
    }
    free(classes);
    return 0;
}