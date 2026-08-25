// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

char *get_rest(char *s, int n) {
    for (int i = 0; i < n; i++) {
        while (*s && *s != ' ') s++;
        if (!*s) return NULL;
        s++;
    }
    return s;
}

typedef struct Stud {
    char *name;
    char *note;
    struct Stud *next;
} Stud;

typedef struct Class {
    char *name;
    Stud *studs;
    struct Class *next;
} Class;

Class *classes = NULL;

Class *find_class(const char *name) {
    for (Class *c = classes; c; c = c->next) {
        if (strcmp(c->name, name) == 0) return c;
    }
    return NULL;
}

void add_class(const char *name) {
    if (find_class(name)) return;
    Class *c = malloc(sizeof(Class));
    c->name = strdup(name);
    c->studs = NULL;
    c->next = classes;
    classes = c;
}

void add_stud(const char *cname, const char *sname, const char *note) {
    Class *c = find_class(cname);
    if (!c) return;
    Stud *s = malloc(sizeof(Stud));
    s->name = strdup(sname);
    s->note = strdup(note);
    s->next = c->studs;
    c->studs = s;
}

Stud *extract_stud(Class *c, const char *sname) {
    Stud *prev = NULL, *curr = c->studs;
    while (curr) {
        if (strcmp(curr->name, sname) == 0) {
            if (prev) prev->next = curr->next;
            else c->studs = curr->next;
            curr->next = NULL;
            return curr;
        }
        prev = curr;
        curr = curr->next;
    }
    return NULL;
}

void transfer_stud(const char *from, const char *to, const char *sname) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Stud *s = extract_stud(cf, sname);
    if (s) {
        s->next = ct->studs;
        ct->studs = s;
    }
}

void drop_stud(const char *cname, const char *sname) {
    Class *c = find_class(cname);
    if (!c) return;
    Stud *s = extract_stud(c, sname);
    if (s) {
        free(s->name);
        free(s->note);
        free(s);
    }
}

void delete_class(const char *name) {
    Class *prev = NULL, *curr = classes;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else classes = curr->next;
            Stud *s = curr->studs;
            while (s) {
                Stud *next = s->next;
                free(s->name);
                free(s->note);
                free(s);
                s = next;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_classes() {
    for (Class *c = classes; c; c = c->next) {
        printf("%s:\n", c->name);
        for (Stud *s = c->studs; s; s = s->next) {
            printf("  %s %s\n", s->name, s->note);
        }
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0}, arg3[256] = {0};
        sscanf(line, "%15s %255s %255s %255s", cmd, arg1, arg2, arg3);
        
        if (strcmp(cmd, "CLASS") == 0) add_class(arg1);
        else if (strcmp(cmd, "STUDENT") == 0) {
            char *note = get_rest(line, 3);
            if (note) add_stud(arg1, arg2, note);
        }
        else if (strcmp(cmd, "TRANSFER") == 0) transfer_stud(arg1, arg2, arg3);
        else if (strcmp(cmd, "DROP") == 0) drop_stud(arg1, arg2);
        else if (strcmp(cmd, "DELETECLASS") == 0) delete_class(arg1);
        else if (strcmp(cmd, "PRINT") == 0) print_classes();
        
        free(line);
    }
    return 0;
}