/* F025.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_students(Student *s) {
    while (s) {
        Student *tmp = s;
        s = s->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

void free_class(Class *c) {
    free(c->name);
    free_students(c->students);
    free(c);
}

void class_create(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void student_add(const char *classname, const char *student, const char *note) {
    Class *c = find_class(classname);
    if (!c) return;
    if (find_student_in_class(c, student)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(student);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void student_transfer(const char *from, const char *to, const char *student) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *prev = NULL, *curr = cf->students;
    while (curr) {
        if (strcmp(curr->name, student) == 0) {
            if (prev) prev->next = curr->next;
            else cf->students = curr->next;
            curr->next = ct->students;
            ct->students = curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void student_drop(const char *classname, const char *student) {
    Class *c = find_class(classname);
    if (!c) return;
    Student *prev = NULL, *curr = c->students;
    while (curr) {
        if (strcmp(curr->name, student) == 0) {
            if (prev) prev->next = curr->next;
            else c->students = curr->next;
            free(curr->name);
            free(curr->note);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void class_delete(const char *name) {
    Class *prev = NULL, *curr = classes;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else classes = curr->next;
            free_class(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_classes(void) {
    Class *c = classes;
    while (c) {
        printf("%s\n", c->name);
        Student *s = c->students;
        while (s) {
            printf("  %s %s\n", s->name, s->note);
            s = s->next;
        }
        c = c->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256], student[256], note[4096], from[256], to[256];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "CLASS") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) class_create(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, student) != 2) continue;
            p += strlen(name) + strlen(student) + 2;
            while (*p == ' ') p++;
            student_add(name, student, p);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s %255s", from, to, student) == 3) student_transfer(from, to, student);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, student) == 2) student_drop(name, student);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *p = line + 11;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) class_delete(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }

    while (classes) {
        Class *tmp = classes;
        classes = classes->next;
        free_class(tmp);
    }
    return 0;
}