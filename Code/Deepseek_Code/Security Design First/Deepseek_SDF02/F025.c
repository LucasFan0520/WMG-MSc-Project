/* F025.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct student {
    char *name;
    char *note;
    struct student *next;
} student_t;

typedef struct class {
    char *name;
    student_t *students;
    struct class *next;
} class_t;

class_t *classes = NULL;

class_t *find_class(const char *name) {
    class_t *c = classes;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

student_t *find_student_in_class(class_t *c, const char *name) {
    student_t *s = c->students;
    while (s) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void class_cmd(const char *name) {
    if (find_class(name)) return;
    class_t *c = malloc(sizeof(class_t));
    if (!c) return;
    c->name = strdup(name);
    c->students = NULL;
    c->next = classes;
    classes = c;
    if (!c->name) { free(c); }
}

void student_cmd(const char *cname, const char *sname, const char *note) {
    class_t *c = find_class(cname);
    if (!c) return;
    if (find_student_in_class(c, sname)) return;
    student_t *s = malloc(sizeof(student_t));
    if (!s) return;
    s->name = strdup(sname);
    s->note = strdup(note);
    s->next = c->students;
    c->students = s;
    if (!s->name || !s->note) {
        free(s->name); free(s->note); free(s);
    }
}

void transfer_cmd(const char *from, const char *to, const char *sname) {
    class_t *fc = find_class(from);
    class_t *tc = find_class(to);
    if (!fc || !tc) return;
    student_t *prev = NULL, *s = fc->students;
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

void drop_cmd(const char *cname, const char *sname) {
    class_t *c = find_class(cname);
    if (!c) return;
    student_t *prev = NULL, *s = c->students;
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

void deleteclass_cmd(const char *name) {
    class_t *prev = NULL, *c = classes;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else classes = c->next;
            free(c->name);
            student_t *s = c->students;
            while (s) {
                student_t *nxt = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = nxt;
            }
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void print_cmd(void) {
    class_t *c = classes;
    while (c) {
        printf("%s\n", c->name);
        student_t *s = c->students;
        while (s) {
            printf("%s %s\n", s->name, s->note);
            s = s->next;
        }
        c = c->next;
    }
}

void free_all(void) {
    class_t *c = classes;
    while (c) {
        class_t *nxt = c->next;
        free(c->name);
        student_t *s = c->students;
        while (s) {
            student_t *n = s->next;
            free(s->name);
            free(s->note);
            free(s);
            s = n;
        }
        free(c);
        c = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[256], b[256], c[256];
        if (sscanf(line, "%19s %255s %255s %255s", cmd, a, b, c) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "CLASS") == 0) {
                class_cmd(a);
            } else if (strcmp(cmd, "STUDENT") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2) {
                    const char *note = line + strlen(cmd) + 1 + strlen(a) + 1 + strlen(b) + 1;
                    student_cmd(a, b, note);
                }
            } else if (strcmp(cmd, "TRANSFER") == 0) {
                if (sscanf(line, "%*s %255s %255s %255s", a, b, c) == 3)
                    transfer_cmd(a, b, c);
            } else if (strcmp(cmd, "DROP") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    drop_cmd(a, b);
            } else if (strcmp(cmd, "DELETECLASS") == 0) {
                deleteclass_cmd(a);
            } else if (strcmp(cmd, "PRINT") == 0) {
                print_cmd();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}