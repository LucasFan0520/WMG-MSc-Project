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
    c->name = malloc(strlen(name) + 1);
    strcpy(c->name, name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void add_student(const char *cname, const char *sname, const char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    if (find_student_in_class(c, sname)) return;
    Student *s = malloc(sizeof(Student));
    s->name = malloc(strlen(sname) + 1);
    strcpy(s->name, sname);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *sname) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = cf->students, *prev = NULL;
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

void drop_student(const char *cname, const char *sname) {
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

void delete_class(const char *name) {
    Class *c = classes, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else classes = c->next;
            free(c->name);
            Student *s = c->students;
            while (s) {
                Student *tmp = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = tmp;
            }
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void print_all(void) {
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

void free_all(void) {
    Class *c = classes;
    while (c) {
        Class *tmp = c->next;
        free(c->name);
        Student *s = c->students;
        while (s) {
            Student *ts = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = ts;
        }
        free(c);
        c = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[256], c[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "CLASS") == 0) {
            if (sscanf(rest, "%255s", a) == 1) create_class(a);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            if (sscanf(rest, "%255s %255s", a, b) == 2) {
                const char *n = rest + strlen(a) + 1 + strlen(b) + 1;
                while (*n == ' ') n++;
                if (*n) add_student(a, b, n);
                else add_student(a, b, "");
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            if (sscanf(rest, "%255s %255s %255s", a, b, c) == 3) {
                transfer_student(a, b, c);
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(rest, "%255s %255s", a, b) == 2) drop_student(a, b);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            if (sscanf(rest, "%255s", a) == 1) delete_class(a);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_all();
        }
    }
    free_all();
    return 0;
}