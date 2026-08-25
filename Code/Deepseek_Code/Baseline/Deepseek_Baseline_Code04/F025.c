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

void add_student(const char *classname, const char *student, const char *note) {
    Class *c = find_class(classname);
    if (!c) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(student);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *student) {
    Class *fc = find_class(from);
    Class *tc = find_class(to);
    if (!fc || !tc) return;
    Student *prev = NULL, *cur = fc->students;
    while (cur) {
        if (strcmp(cur->name, student) == 0) {
            if (prev) prev->next = cur->next;
            else fc->students = cur->next;
            cur->next = tc->students;
            tc->students = cur;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void drop_student(const char *classname, const char *student) {
    Class *c = find_class(classname);
    if (!c) return;
    Student *prev = NULL, *cur = c->students;
    while (cur) {
        if (strcmp(cur->name, student) == 0) {
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
    Class *prev = NULL, *cur = classes;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else classes = cur->next;
            free(cur->name);
            Student *s = cur->students;
            while (s) {
                Student *ts = s;
                s = s->next;
                free(ts->name);
                free(ts->note);
                free(ts);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
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

void free_classes(void) {
    while (classes) {
        Class *tmp = classes;
        classes = classes->next;
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256], student[256];
        int n = sscanf(line, "%s %s %s", cmd, name, student);
        if (strcmp(cmd, "CLASS") == 0) {
            if (n < 2) continue;
            add_class(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            if (n < 3) continue;
            char *p = line + strlen(cmd) + 1 + strlen(name) + 1 + strlen(student) + 1;
            while (*p == ' ') p++;
            if (*p) add_student(name, student, p);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            if (n < 3) continue;
            char to[256];
            sscanf(line, "%*s %s %s %s", name, to, student);
            transfer_student(name, to, student);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (n < 3) continue;
            drop_student(name, student);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            if (n < 2) continue;
            delete_class(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free_classes();
    return 0;
}