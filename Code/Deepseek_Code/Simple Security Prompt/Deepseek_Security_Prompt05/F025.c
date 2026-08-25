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
    if (!c) return;
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void student_add(const char *cname, const char *sname, const char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    if (find_student(c, sname)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(sname);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void student_transfer(const char *from, const char *to, const char *sname) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = find_student(cf, sname);
    if (!s) return;
    Student *cur = cf->students, *prev = NULL;
    while (cur) {
        if (cur == s) {
            if (prev) prev->next = cur->next;
            else cf->students = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    s->next = ct->students;
    ct->students = s;
}

void student_drop(const char *cname, const char *sname) {
    Class *c = find_class(cname);
    if (!c) return;
    Student *s = c->students, *prev = NULL;
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
    Class *c = classes, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else classes = c->next;
            free(c->name);
            while (c->students) {
                Student *tmp = c->students;
                c->students = c->students->next;
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256], arg3[2048];
        int n = sscanf(line, "%15s %255s %255s %2047s", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "CLASS") == 0 && n >= 2) {
            class_create(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0 && n >= 3) {
            char *note = line + strlen(cmd) + strlen(arg1) + strlen(arg2) + 3;
            while (*note == ' ') note++;
            student_add(arg1, arg2, note);
        } else if (strcmp(cmd, "TRANSFER") == 0 && n >= 4) {
            student_transfer(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 3) {
            student_drop(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0 && n >= 2) {
            class_delete(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    while (classes) {
        Class *tmp = classes;
        classes = classes->next;
        free(tmp->name);
        while (tmp->students) {
            Student *s = tmp->students;
            tmp->students = tmp->students->next;
            free(s->name);
            free(s->note);
            free(s);
        }
        free(tmp);
    }
    return 0;
}