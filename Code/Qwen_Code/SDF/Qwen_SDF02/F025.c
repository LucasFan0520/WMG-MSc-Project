// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

Class* find_class(const char *name) {
    for (Class *c = classes; c; c = c->next)
        if (strcmp(c->name, name) == 0) return c;
    return NULL;
}

Student* find_student(Class *c, const char *name) {
    for (Student *s = c->students; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

Student* remove_student(Class *c, const char *name) {
    Student *prev = NULL;
    for (Student *s = c->students; s; prev = s, s = s->next) {
        if (strcmp(s->name, name) == 0) {
            if (prev) prev->next = s->next;
            else c->students = s->next;
            s->next = NULL;
            return s;
        }
    }
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "CLASS") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            if (find_class(name)) continue;
            Class *c = malloc(sizeof(Class));
            if (!c) continue;
            c->name = strdup(name);
            if (!c->name) { free(c); continue; }
            c->students = NULL;
            c->next = classes;
            classes = c;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cls[256], stu[256];
            int n = 0;
            if (sscanf(rest, "%255s %255s%n", cls, stu, &n) != 2) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            Class *c = find_class(cls);
            if (!c || find_student(c, stu)) continue;
            Student *s = malloc(sizeof(Student));
            if (!s) continue;
            s->name = strdup(stu);
            s->note = strdup(note);
            if (!s->name || !s->note) { free(s->name); free(s->note); free(s); continue; }
            s->next = c->students;
            c->students = s;
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256], to[256], stu[256];
            if (sscanf(rest, "%255s %255s %255s", from, to, stu) != 3) continue;
            Class *c1 = find_class(from);
            Class *c2 = find_class(to);
            if (!c1 || !c2) continue;
            Student *s = remove_student(c1, stu);
            if (s) {
                s->next = c2->students;
                c2->students = s;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char cls[256], stu[256];
            if (sscanf(rest, "%255s %255s", cls, stu) != 2) continue;
            Class *c = find_class(cls);
            if (c) {
                Student *s = remove_student(c, stu);
                if (s) {
                    free(s->name);
                    free(s->note);
                    free(s);
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Class *prev = NULL;
            for (Class *c = classes; c; prev = c, c = c->next) {
                if (strcmp(c->name, name) == 0) {
                    if (prev) prev->next = c->next;
                    else classes = c->next;
                    while (c->students) {
                        Student *s = c->students;
                        c->students = s->next;
                        free(s->name);
                        free(s->note);
                        free(s);
                    }
                    free(c->name);
                    free(c);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Class *c = classes; c; c = c->next) {
                printf("%s\n", c->name);
                for (Student *s = c->students; s; s = s->next) {
                    printf("  %s ", s->name);
                    print_text(s->note);
                }
            }
        }
    }
    while (classes) {
        Class *c = classes;
        classes = classes->next;
        while (c->students) {
            Student *s = c->students;
            c->students = s->next;
            free(s->name);
            free(s->note);
            free(s);
        }
        free(c->name);
        free(c);
    }
    return 0;
}