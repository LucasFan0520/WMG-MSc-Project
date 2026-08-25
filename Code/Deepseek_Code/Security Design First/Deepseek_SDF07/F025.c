// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    char *note;
    struct Student *next;
} Student;

typedef struct {
    char *name;
    Student *head;
    size_t count;
} Class;

typedef struct {
    Class *items;
    size_t count;
    size_t capacity;
} Roster;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void student_free(Student *head) {
    while (head) {
        Student *next = head->next;
        free(head->name);
        free(head->note);
        free(head);
        head = next;
    }
}

static void class_free(Class *c) {
    free(c->name);
    student_free(c->head);
    c->head = NULL;
    c->count = 0;
}

static void roster_init(Roster *r) {
    r->items = NULL;
    r->count = 0;
    r->capacity = 0;
}

static void roster_free(Roster *r) {
    for (size_t i = 0; i < r->count; i++)
        class_free(&r->items[i]);
    free(r->items);
    r->items = NULL;
    r->count = 0;
    r->capacity = 0;
}

static int roster_find_class(Roster *r, const char *name) {
    for (size_t i = 0; i < r->count; i++) {
        if (strcmp(r->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static Student *roster_find_student(Roster *r, int idx, const char *student) {
    Student *cur = r->items[idx].head;
    while (cur) {
        if (strcmp(cur->name, student) == 0)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

static void roster_class(Roster *r, const char *name) {
    if (roster_find_class(r, name) >= 0) return;
    if (r->count == r->capacity) {
        size_t newcap = r->capacity ? r->capacity * 2 : 4;
        Class *tmp = realloc(r->items, newcap * sizeof(Class));
        if (!tmp) return;
        r->items = tmp;
        r->capacity = newcap;
    }
    r->items[r->count].name = safe_strdup(name);
    r->items[r->count].head = NULL;
    r->items[r->count].count = 0;
    if (r->items[r->count].name)
        r->count++;
    else
        free(r->items[r->count].name);
}

static void roster_student(Roster *r, const char *classname, const char *student, const char *note) {
    int idx = roster_find_class(r, classname);
    if (idx < 0) return;
    if (roster_find_student(r, idx, student)) return;
    Student *s = malloc(sizeof(Student));
    if (!s) return;
    s->name = safe_strdup(student);
    s->note = safe_strdup(note);
    s->next = r->items[idx].head;
    if (s->name && s->note) {
        r->items[idx].head = s;
        r->items[idx].count++;
    } else {
        free(s->name);
        free(s->note);
        free(s);
    }
}

static void roster_transfer(Roster *r, const char *from, const char *to, const char *student) {
    int fi = roster_find_class(r, from);
    int ti = roster_find_class(r, to);
    if (fi < 0 || ti < 0) return;
    Student **ptr = &r->items[fi].head;
    Student *found = NULL;
    while (*ptr) {
        if (strcmp((*ptr)->name, student) == 0) {
            found = *ptr;
            *ptr = found->next;
            r->items[fi].count--;
            break;
        }
        ptr = &(*ptr)->next;
    }
    if (!found) return;
    found->next = r->items[ti].head;
    r->items[ti].head = found;
    r->items[ti].count++;
}

static void roster_drop(Roster *r, const char *classname, const char *student) {
    int idx = roster_find_class(r, classname);
    if (idx < 0) return;
    Student **ptr = &r->items[idx].head;
    while (*ptr) {
        if (strcmp((*ptr)->name, student) == 0) {
            Student *del = *ptr;
            *ptr = del->next;
            free(del->name);
            free(del->note);
            free(del);
            r->items[idx].count--;
            return;
        }
        ptr = &(*ptr)->next;
    }
}

static void roster_deleteclass(Roster *r, const char *name) {
    int idx = roster_find_class(r, name);
    if (idx < 0) return;
    class_free(&r->items[idx]);
    for (size_t i = idx; i < r->count - 1; i++)
        r->items[i] = r->items[i+1];
    r->count--;
}

static void roster_print(Roster *r) {
    for (size_t i = 0; i < r->count; i++) {
        printf("%s:\n", r->items[i].name);
        Student *cur = r->items[i].head;
        while (cur) {
            printf("  %s ", cur->name);
            const char *p = cur->note;
            while (*p) {
                putchar(*p == ' ' ? '_' : *p);
                p++;
            }
            putchar('\n');
            cur = cur->next;
        }
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz, char *a3, size_t a3sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a3sz - 1) a3[i++] = *p++;
    a3[i] = '\0';
}

int main(void) {
    Roster r;
    roster_init(&r);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256], a3[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2), a3, sizeof(a3));
        if (strcmp(cmd, "CLASS") == 0) {
            roster_class(&r, a1);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            roster_student(&r, a1, a2, a3);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            roster_transfer(&r, a1, a2, a3);
        } else if (strcmp(cmd, "DROP") == 0) {
            roster_drop(&r, a1, a2);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            roster_deleteclass(&r, a1);
        } else if (strcmp(cmd, "PRINT") == 0) {
            roster_print(&r);
        }
    }
    roster_free(&r);
    return 0;
}