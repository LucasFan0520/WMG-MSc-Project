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
    if (!c) return;
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void add_student(const char *class, const char *student, const char *note) {
    Class *c = find_class(class);
    if (!c) return;
    if (find_student_in_class(c, student)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
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
            // add to target
            cur->next = tc->students;
            tc->students = cur;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void drop_student(const char *class, const char *student) {
    Class *c = find_class(class);
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
            Student *s = cur->students;
            while (s) {
                Student *next = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = next;
            }
            free(cur->name);
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
        printf("%s:\n", c->name);
        Student *s = c->students;
        while (s) {
            printf("%s: ", s->name);
            char *n = s->note;
            for (; *n; n++) putchar(*n == ' ' ? '_' : *n);
            putchar('\n');
            s = s->next;
        }
        c = c->next;
    }
}

void free_all(void) {
    Class *c = classes;
    while (c) {
        Class *next = c->next;
        Student *s = c->students;
        while (s) {
            Student *snext = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = snext;
        }
        free(c->name);
        free(c);
        c = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            create_class(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *class = p;
            char *space = strchr(class, ' ');
            if (!space) continue;
            *space = '\0';
            char *student = space + 1;
            while (*student == ' ') student++;
            space = strchr(student, ' ');
            if (!space) continue;
            *space = '\0';
            char *note = space + 1;
            while (*note == ' ') note++;
            add_student(class, student, note);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *from = p;
            char *space = strchr(from, ' ');
            if (!space) continue;
            *space = '\0';
            char *to = space + 1;
            while (*to == ' ') to++;
            space = strchr(to, ' ');
            if (!space) continue;
            *space = '\0';
            char *student = space + 1;
            while (*student == ' ') student++;
            transfer_student(from, to, student);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *class = p;
            char *space = strchr(class, ' ');
            if (!space) continue;
            *space = '\0';
            char *student = space + 1;
            while (*student == ' ') student++;
            drop_student(class, student);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *p = line + 13;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            delete_class(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free_all();
    return 0;
}