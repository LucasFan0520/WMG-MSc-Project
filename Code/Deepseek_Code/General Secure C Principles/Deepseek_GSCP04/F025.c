/* F025.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Student {
    char *name;
    char *note;
} Student;

typedef struct Class {
    char *name;
    Student *students;
    size_t scount, scap;
} Class;

static Class *classes = NULL;
static size_t ccount = 0, ccap = 0;

static Class *find_class(const char *name) {
    for (size_t i = 0; i < ccount; i++) {
        if (strcmp(classes[i].name, name) == 0)
            return &classes[i];
    }
    return NULL;
}

static void add_class(const char *name) {
    if (ccount == ccap) {
        ccap = ccap ? ccap * 2 : 4;
        classes = realloc(classes, ccap * sizeof(Class));
        if (!classes) { perror("realloc"); exit(1); }
    }
    classes[ccount].name = strdup(name);
    classes[ccount].students = NULL;
    classes[ccount].scount = classes[ccount].scap = 0;
    ccount++;
}

static Student *find_student_in_class(Class *c, const char *name) {
    for (size_t i = 0; i < c->scount; i++) {
        if (strcmp(c->students[i].name, name) == 0)
            return &c->students[i];
    }
    return NULL;
}

static void add_student(Class *c, const char *name, const char *note) {
    if (find_student_in_class(c, name)) return;
    if (c->scount == c->scap) {
        c->scap = c->scap ? c->scap * 2 : 4;
        c->students = realloc(c->students, c->scap * sizeof(Student));
        if (!c->students) { perror("realloc"); exit(1); }
    }
    c->students[c->scount].name = strdup(name);
    c->students[c->scount].note = strdup(note);
    c->scount++;
}

static void remove_student(Class *c, const char *name) {
    for (size_t i = 0; i < c->scount; i++) {
        if (strcmp(c->students[i].name, name) == 0) {
            free(c->students[i].name);
            free(c->students[i].note);
            for (size_t j = i; j < c->scount - 1; j++)
                c->students[j] = c->students[j + 1];
            c->scount--;
            return;
        }
    }
}

static void transfer_student(const char *from, const char *to, const char *name) {
    Class *cf = find_class(from);
    Class *ct = find_class(to);
    if (!cf || !ct) return;
    Student *s = find_student_in_class(cf, name);
    if (!s) return;
    char *note = strdup(s->note);
    add_student(ct, name, note);
    free(note);
    remove_student(cf, name);
}

static void delete_class(const char *name) {
    for (size_t i = 0; i < ccount; i++) {
        if (strcmp(classes[i].name, name) == 0) {
            free(classes[i].name);
            for (size_t j = 0; j < classes[i].scount; j++) {
                free(classes[i].students[j].name);
                free(classes[i].students[j].note);
            }
            free(classes[i].students);
            for (size_t j = i; j < ccount - 1; j++)
                classes[j] = classes[j + 1];
            ccount--;
            return;
        }
    }
}

static void print_classes(void) {
    for (size_t i = 0; i < ccount; i++) {
        printf("%s\n", classes[i].name);
        for (size_t j = 0; j < classes[i].scount; j++) {
            printf("  %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
        }
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            if (!find_class(name)) add_class(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cname = strtok(NULL, " ");
            if (!cname) continue;
            char *sname = strtok(NULL, " ");
            if (!sname) continue;
            char *note = strtok(NULL, "");
            if (!note) note = "";
            Class *c = find_class(cname);
            if (c) add_student(c, sname, note);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *from = strtok(NULL, " ");
            char *to = strtok(NULL, " ");
            char *sname = strtok(NULL, " ");
            if (from && to && sname) transfer_student(from, to, sname);
        } else if (strcmp(cmd, "DROP") == 0) {
            char *cname = strtok(NULL, " ");
            if (!cname) continue;
            char *sname = strtok(NULL, " ");
            if (!sname) continue;
            Class *c = find_class(cname);
            if (c) remove_student(c, sname);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_class(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            print_classes();
        }
    }
    for (size_t i = 0; i < ccount; i++) {
        free(classes[i].name);
        for (size_t j = 0; j < classes[i].scount; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}