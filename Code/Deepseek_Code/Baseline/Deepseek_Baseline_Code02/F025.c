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

Class *head = NULL;

Class *find_class(const char *name) {
    Class *c = head;
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
    c->next = head;
    head = c;
}

void add_student(const char *class_name, const char *student_name, const char *note) {
    Class *c = find_class(class_name);
    if (!c) return;
    if (find_student_in_class(c, student_name)) return;
    Student *s = malloc(sizeof(Student));
    s->name = malloc(strlen(student_name) + 1);
    strcpy(s->name, student_name);
    s->note = malloc(strlen(note) + 1);
    strcpy(s->note, note);
    s->next = c->students;
    c->students = s;
}

void transfer_student(const char *from, const char *to, const char *student_name) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *prev = NULL;
    Student *s = cf->students;
    while (s) {
        if (strcmp(s->name, student_name) == 0) {
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
    Class *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free(c->name);
            Student *s = c->students;
            while (s) {
                Student *next = s->next;
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

void print_classes(void) {
    Class *c = head;
    while (c) {
        printf("%s:\n", c->name);
        Student *s = c->students;
        while (s) {
            printf("  %s ", s->name);
            for (char *p = s->note; *p; p++) {
                putchar(*p == ' ' ? '_' : *p);
            }
            putchar('\n');
            s = s->next;
        }
        c = c->next;
    }
}

void free_all(void) {
    Class *c = head;
    while (c) {
        Class *nextc = c->next;
        free(c->name);
        Student *s = c->students;
        while (s) {
            Student *nexts = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = nexts;
        }
        free(c);
        c = nextc;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            create_class(p);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *class_name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *student = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_student(class_name, student, p);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *from = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            char *to = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            transfer_student(from, to, p);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *class_name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            drop_student(class_name, p);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            delete_class(p);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free_all();
    return 0;
}