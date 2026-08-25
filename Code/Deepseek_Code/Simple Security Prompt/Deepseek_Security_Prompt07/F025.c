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

void create_class(const char *name) {
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
    Class *fc = find_class(from);
    Class *tc = find_class(to);
    if (!fc || !tc) return;
    Student *prev = NULL;
    Student *s = fc->students;
    while (s) {
        if (strcmp(s->name, student_name) == 0) {
            if (prev) prev->next = s->next;
            else fc->students = s->next;
            s->next = tc->students;
            tc->students = s;
            return;
        }
        prev = s;
        s = s->next;
    }
}

void drop_student(const char *class_name, const char *student_name) {
    Class *c = find_class(class_name);
    if (!c) return;
    Student *prev = NULL;
    Student *s = c->students;
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
    Class *prev = NULL;
    Class *c = classes;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else classes = c->next;
            Student *s = c->students;
            while (s) {
                Student *next = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = next;
            }
            free(c->name);
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
            printf("  %s:", s->name);
            for (char *p = s->note; *p; p++) putchar(*p == ' ' ? '_' : *p);
            putchar('\n');
            s = s->next;
        }
        c = c->next;
    }
}

void free_all() {
    Class *c = classes;
    while (c) {
        Class *nextc = c->next;
        Student *s = c->students;
        while (s) {
            Student *nexts = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = nexts;
        }
        free(c->name);
        free(c);
        c = nextc;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "CLASS") == 0) {
            create_class(rest);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cname[256], sname[256], note[4096];
            if (sscanf(rest, "%255s %255s %[^\n]", cname, sname, note) == 3) {
                add_student(cname, sname, note);
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256], to[256], sname[256];
            if (sscanf(rest, "%255s %255s %255s", from, to, sname) == 3) {
                transfer_student(from, to, sname);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char cname[256], sname[256];
            if (sscanf(rest, "%255s %255s", cname, sname) == 2) {
                drop_student(cname, sname);
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            delete_class(rest);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free_all();
    return 0;
}