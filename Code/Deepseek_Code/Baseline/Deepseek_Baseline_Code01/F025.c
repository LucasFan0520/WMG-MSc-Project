// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student {
    char *name;
    char *note;
    struct student *next;
} Student;

typedef struct class {
    char *name;
    Student *students;
    struct class *next;
} Class;

Class *classes = NULL;

Class *find_class(const char *name) {
    Class *c = classes;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

Student *find_student_in_class(Class *c, const char *name) {
    Student *s = c->students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void add_student_to_class(Class *c, const char *name, const char *note) {
    if (find_student_in_class(c, name)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(name);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void remove_student_from_class(Class *c, const char *name) {
    Student *s = c->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, name) == 0) {
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

void class_create(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void delete_class(const char *name) {
    Class *c = classes, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else classes = c->next;
            free(c->name);
            Student *s = c->students;
            while (s) { Student *tmp = s; s = s->next; free(tmp->name); free(tmp->note); free(tmp); }
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void transfer_student(const char *from, const char *to, const char *name) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = find_student_in_class(cf, name);
    if (!s || find_student_in_class(ct, name)) return;
    remove_student_from_class(cf, name);
    add_student_to_class(ct, name, s->note);
    free(s->name);
    free(s->note);
    free(s);
}

void drop_student(const char *class, const char *name) {
    Class *c = find_class(class);
    if (c) remove_student_from_class(c, name);
}

void print_classes(void) {
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                class_create(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            if (sscanf(line, "%*s %255s %255s %[^\n]", arg1, arg2, arg3) == 3) {
                Class *c = find_class(arg1);
                if (c) add_student_to_class(c, arg2, arg3);
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", arg1, arg2, arg3) == 3)
                transfer_student(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                drop_student(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                delete_class(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    return 0;
}