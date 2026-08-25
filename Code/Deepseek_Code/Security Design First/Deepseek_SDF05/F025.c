// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void class_create(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    if (!c) return;
    c->name = safe_strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
}

void student_add(const char *class_name, const char *student_name, const char *note) {
    Class *c = find_class(class_name);
    if (!c) return;
    if (find_student(c, student_name)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = safe_strdup(student_name);
    s->note = safe_strdup(note);
    s->next = c->students;
    c->students = s;
}

void student_transfer(const char *from, const char *to, const char *student_name) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = find_student(cf, student_name);
    if (!s) return;
    // Remove from cf
    Student *prev = NULL, *cur = cf->students;
    while (cur) {
        if (cur == s) {
            if (prev) prev->next = cur->next;
            else cf->students = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    // Add to ct
    s->next = ct->students;
    ct->students = s;
}

void student_drop(const char *class_name, const char *student_name) {
    Class *c = find_class(class_name);
    if (!c) return;
    Student *prev = NULL, *cur = c->students;
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

void class_delete(const char *name) {
    Class *prev = NULL, *cur = classes;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else classes = cur->next;
            free(cur->name);
            Student *s = cur->students;
            while (s) {
                Student *next = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void class_print(void) {
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

void free_all(void) {
    Class *c = classes;
    while (c) {
        Class *next_c = c->next;
        free(c->name);
        Student *s = c->students;
        while (s) {
            Student *next_s = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = next_s;
        }
        free(c);
        c = next_c;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512], arg3[512];
        int n = sscanf(line, "%31s %511s %511s %511s", cmd, arg1, arg2, arg3);
        if (n < 1) continue;

        if (strcmp(cmd, "CLASS") == 0 && n >= 2) {
            class_create(arg1);
        } else if (strcmp(cmd, "STUDENT") == 0 && n >= 3) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            student_add(arg1, arg2, p);
        } else if (strcmp(cmd, "TRANSFER") == 0 && n >= 4) {
            student_transfer(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DROP") == 0 && n >= 3) {
            student_drop(arg1, arg2);
        } else if (strcmp(cmd, "DELETECLASS") == 0 && n >= 2) {
            class_delete(arg1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            class_print();
        }
    }
    free_all();
    return 0;
}