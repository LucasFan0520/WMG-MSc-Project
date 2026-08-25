/*
 * F025.c
 */
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

Student *find_student(Class *c, const char *name) {
    Student *s = c->students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void class_create(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void class_add_student(const char *cname, const char *sname, const char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    if (find_student(c, sname)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(sname);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void class_transfer(const char *from, const char *to, const char *sname) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *prev = NULL, *s = cf->students;
    while (s) {
        if (strcmp(s->name, sname) == 0) {
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

void class_drop(const char *cname, const char *sname) {
    Class *c = find_class(cname);
    if (!c) return;
    Student *prev = NULL, *s = c->students;
    while (s) {
        if (strcmp(s->name, sname) == 0) {
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

void class_delete(const char *name) {
    Class *prev = NULL, *c = classes;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else classes = c->next;
            Student *s = c->students;
            while (s) {
                Student *tmp = s;
                s = s->next;
                free(tmp->name);
                free(tmp->note);
                free(tmp);
            }
            free(c->name);
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void class_print(void) {
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
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], arg3[256];
        if (sscanf(line, "%31s %255s %255s %255s", cmd, arg1, arg2, arg3) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "CLASS") == 0) {
            class_create(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            class_add_student(arg1, arg2, rest + strlen(arg2) + 1);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            class_transfer(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0) {
            class_drop(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            class_delete(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            class_print();
        }
    }
    return 0;
}