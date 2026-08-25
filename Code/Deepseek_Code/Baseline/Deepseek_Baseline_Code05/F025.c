/* F025.c */
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

void create_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void add_student(const char *cname, const char *sname, const char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    if (find_student_in_class(c, sname)) return;
    Student *s = malloc(sizeof(Student));
    s->name = strdup(sname);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *sname) {
    Class *fc = find_class(from);
    Class *tc = find_class(to);
    if (!fc || !tc) return;
    Student *s = fc->students, *prev = NULL;
    while (s) {
        if (strcmp(s->name, sname) == 0) {
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
            while (s) { Student *tmp = s; s = s->next; free(tmp->name); free(tmp->note); free(tmp); }
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
            printf("%s ", s->name);
            for (char *p = s->note; *p; p++) putchar(*p == ' ' ? '_' : *p);
            putchar('\n');
            s = s->next;
        }
        c = c->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], c1[256], c2[256], sname[256], note[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "PRINT") == 0) { print_classes(); continue; }
        if (strcmp(cmd, "CLASS") == 0) {
            if (sscanf(line, "%*s %255s", c1) == 1) create_class(c1);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            if (sscanf(line, "%*s %255s %255s", c1, sname) < 2) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(c1) + 1 + strlen(sname) + 1;
            if (*rest == ' ') rest++;
            add_student(c1, sname, rest);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", c1, c2, sname) == 3)
                transfer_student(c1, c2, sname);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (sscanf(line, "%*s %255s %255s", c1, sname) == 2)
                drop_student(c1, sname);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            if (sscanf(line, "%*s %255s", c1) == 1) delete_class(c1);
        }
    }
    return 0;
}