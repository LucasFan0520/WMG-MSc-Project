// F025.c
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

Student *find_student_in_class(Class *c, const char *name) {
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
    if (find_student_in_class(c, student_name)) return;
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
    Student *s = find_student_in_class(cf, student_name);
    if (!s) return;
    if (find_student_in_class(ct, student_name)) return;
    // remove from cf
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
    // add to ct
    s->next = ct->students;
    ct->students = s;
}

void drop_student(const char *class_name, const char *student_name) {
    Class *c = find_class(class_name);
    if (!c) return;
    Student *cur = c->students, *prev = NULL;
    while (cur) {
        if (strcmp(cur->name, student_name) == 0) {
            if (prev) prev->next = cur->next;
            else c->students = cur->next;
            free(cur->name);
            free(cur->note);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_class(const char *name) {
    Class *c = find_class(name);
    if (!c) return;
    Class *cur = classes, *prev = NULL;
    while (cur) {
        if (cur == c) {
            if (prev) prev->next = cur->next;
            else classes = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
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

void cleanup_classes(void) {
    Class *c = classes;
    while (c) {
        Class *tmp = c;
        c = c->next;
        free(tmp->name);
        Student *s = tmp->students;
        while (s) {
            Student *ts = s;
            s = s->next;
            free(ts->name);
            free(ts->note);
            free(ts);
        }
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[256], rest[2048];
        int n = sscanf(line, "%31s %255s %255s %2047s", cmd, arg1, arg2, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            add_class(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0 && n >= 3) {
            char *note = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(arg2) + 1;
            add_student(arg1, arg2, note);
        } else if (strcmp(cmd, "TRANSFER") == 0 && n >= 4) {
            transfer_student(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 3) {
            drop_student(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            delete_class(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    cleanup_classes();
    return 0;
}