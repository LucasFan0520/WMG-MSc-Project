// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student {
    char *name;
    char *note;
    struct student *next;
};

struct class {
    char *name;
    struct student *students;
    struct class *next;
};

struct class *classes = NULL;

struct class *find_class(char *name) {
    struct class *c = classes;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

struct student *find_student_in_class(struct class *c, char *name) {
    struct student *s = c->students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

struct student *find_student_anywhere(char *name) {
    struct class *c = classes;
    while (c) {
        struct student *s = find_student_in_class(c, name);
        if (s) return s;
        c = c->next;
    }
    return NULL;
}

void create_class(char *name) {
    if (find_class(name)) return;
    struct class *c = malloc(sizeof(struct class));
    c->name = malloc(strlen(name) + 1);
    strcpy(c->name, name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void add_student(char *classname, char *student, char *note) {
    struct class *c = find_class(classname);
    if (!c) return;
    if (find_student_in_class(c, student)) return;
    struct student *s = malloc(sizeof(struct student));
    s->name = malloc(strlen(student) + 1);
    strcpy(s->name, student);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(char *from, char *to, char *student) {
    struct class *cf = find_class(from);
    struct class *ct = find_class(to);
    if (!cf || !ct) return;
    struct student *s = find_student_in_class(cf, student);
    if (!s) return;
    if (find_student_in_class(ct, student)) return;
    // remove from cf
    struct student *cur = cf->students, *prev = NULL;
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

void drop_student(char *classname, char *student) {
    struct class *c = find_class(classname);
    if (!c) return;
    struct student *s = find_student_in_class(c, student);
    if (!s) return;
    struct student *cur = c->students, *prev = NULL;
    while (cur) {
        if (cur == s) {
            if (prev) prev->next = cur->next;
            else c->students = cur->next;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void delete_class(char *name) {
    struct class *c = classes, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else classes = c->next;
            free(c->name);
            struct student *s = c->students;
            while (s) {
                struct student *next = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = next;
            }
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void print_classes() {
    struct class *c = classes;
    while (c) {
        printf("%s\n", c->name);
        struct student *s = c->students;
        while (s) {
            printf("%s %s\n", s->name, s->note);
            s = s->next;
        }
        c = c->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[256], arg3[4096];
        int n = sscanf(line, "%31s %255s %255s %4095[^\n]", cmd, arg1, arg2, arg3);
        if (strcmp(cmd, "CLASS") == 0 && n >= 2) {
            create_class(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0 && n >= 3) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1 + strlen(arg2) + 1;
            add_student(arg1, arg2, rest);
        } else if (strcmp(cmd, "TRANSFER") == 0 && n >= 4) {
            transfer_student(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 3) {
            drop_student(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0 && n >= 2) {
            delete_class(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    return 0;
}