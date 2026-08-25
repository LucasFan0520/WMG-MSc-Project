/* F025.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    char *name;
    char *note;
    struct Student *next;
} Student;

typedef struct Class {
    char *name;
    Student *students;
    struct Class *next;
} Class;

Class *classes = NULL;

Class* find_class(const char *name) {
    Class *c = classes;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

Student* find_student(Class *c, const char *name) {
    Student *s = c->students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void add_student(const char *class_name, const char *student_name, const char *note) {
    Class *c = find_class(class_name);
    if (!c) return;
    if (find_student(c, student_name)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(student_name);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *student_name) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = cf->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, student_name) == 0) {
            if (prev) prev->next = s->next;
            else cf->students = s->next;
            s->next = ct->students;
            ct->students = s;
            return;
        }
        prev = s;
        s = s->next;
    }
}

void drop_student(const char *class_name, const char *student_name) {
    Class *c = find_class(class_name);
    if (!c) return;
    Student *s = c->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, student_name) == 0) {
            if (prev) prev->next = s->next;
            else c->students = s->next;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
        s = s->next;
    }
}

void delete_class(const char *name) {
    Class *c = classes, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else classes = c->next;
            free(c->name);
            Student *s = c->students;
            while (s) {
                Student *tmp = s;
                s = s->next;
                free(tmp->name);
                free(tmp->note);
                free(tmp);
            }
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void print_classes() {
    Class *c = classes;
    while (c) {
        printf("%s\n", c->name);
        Student *s = c->students;
        while (s) {
            printf("%s %s\n", s->name, s->note);
            s = s->next;
        }
        c = c->next;
    }
}

void free_all() {
    while (classes) {
        Class *tmp = classes;
        classes = classes->next;
        free(tmp->name);
        Student *s = tmp->students;
        while (s) {
            Student *stmp = s;
            s = s->next;
            free(stmp->name);
            free(stmp->note);
            free(stmp);
        }
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[256], c[4096];
        int n = sscanf(line, "%31s %255s %255s %[^\n]", cmd, a, b, c);
        if (strcmp(cmd, "CLASS") == 0 && n >= 2) {
            add_class(a);
        } else if (strcmp(cmd, "STUDENT") == 0 && n >= 3) {
            add_student(a, b, c);
        } else if (strcmp(cmd, "TRANSFER") == 0 && n >= 4) {
            transfer_student(a, b, c);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 3) {
            drop_student(a, b);
        } else if (strcmp(cmd, "DELETECLASS") == 0 && n >= 2) {
            delete_class(a);
        } else if (strcmp(cmd, "PRINT") == 0 && n == 1) {
            print_classes();
        }
    }
    free_all();
    return 0;
}