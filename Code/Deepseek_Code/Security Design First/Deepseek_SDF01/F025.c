// filename: F025.c
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

static Class *classes = NULL;

static void free_students(Student *s) {
    while (s) {
        Student *tmp = s;
        s = s->next;
        free(tmp->name);
        free(tmp->note);
        free(tmp);
    }
}

static void cleanup(void) {
    while (classes) {
        Class *tmp = classes;
        classes = classes->next;
        free(tmp->name);
        free_students(tmp->students);
        free(tmp);
    }
}

static Class *find_class(const char *name) {
    for (Class *c = classes; c; c = c->next)
        if (strcmp(c->name, name) == 0)
            return c;
    return NULL;
}

static Student *find_student_in_class(Class *c, const char *name) {
    for (Student *s = c->students; s; s = s->next)
        if (strcmp(s->name, name) == 0)
            return s;
    return NULL;
}

static Student *find_student_anywhere(const char *name, Class **out_class) {
    for (Class *c = classes; c; c = c->next) {
        Student *s = find_student_in_class(c, name);
        if (s) {
            *out_class = c;
            return s;
        }
    }
    return NULL;
}

static void class_create(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    if (!c) return;
    c->name = strdup(name);
    if (!c->name) { free(c); return; }
    c->students = NULL;
    c->next = classes;
    classes = c;
}

static void student_add(const char *class_name, const char *student_name, const char *note) {
    Class *c = find_class(class_name);
    if (!c) return;
    if (find_student_in_class(c, student_name)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = strdup(student_name);
    s->note = strdup(note);
    if (!s->name || !s->note) {
        free(s->name);
        free(s->note);
        free(s);
        return;
    }
    s->next = c->students;
    c->students = s;
}

static void student_transfer(const char *from, const char *to, const char *student_name) {
    Class *fc = find_class(from);
    Class *tc = find_class(to);
    if (!fc || !tc) return;
    Student *prev = NULL;
    Student *s = NULL;
    for (Student *cur = fc->students; cur; cur = cur->next) {
        if (strcmp(cur->name, student_name) == 0) {
            s = cur;
            break;
        }
        prev = cur;
    }
    if (!s) return;
    if (prev) prev->next = s->next;
    else fc->students = s->next;
    s->next = tc->students;
    tc->students = s;
}

static void student_drop(const char *class_name, const char *student_name) {
    Class *c = find_class(class_name);
    if (!c) return;
    Student *prev = NULL;
    for (Student *s = c->students; s; s = s->next) {
        if (strcmp(s->name, student_name) == 0) {
            if (prev) prev->next = s->next;
            else c->students = s->next;
            free(s->name);
            free(s->note);
            free(s);
            return;
        }
        prev = s;
    }
}

static void class_delete(const char *name) {
    Class *prev = NULL;
    for (Class *c = classes; c; c = c->next) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else classes = c->next;
            free(c->name);
            free_students(c->students);
            free(c);
            return;
        }
        prev = c;
    }
}

static void print_classes(void) {
    for (Class *c = classes; c; c = c->next) {
        printf("%s:", c->name);
        for (Student *s = c->students; s; s = s->next)
            printf(" %s(%s)", s->name, s->note);
        putchar('\n');
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "CLASS") == 0) {
            if (*arg1) class_create(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            if (*arg1 && *arg2) student_add(arg1, arg2, arg3);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            if (*arg1 && *arg2 && *arg3) student_transfer(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0) {
            if (*arg1 && *arg2) student_drop(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            if (*arg1) class_delete(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    free(line);
    return 0;
}