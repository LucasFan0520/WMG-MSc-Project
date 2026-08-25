// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 8192

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

typedef struct { char *name; char *note; } Student;
typedef struct {
    char *name;
    Student *students;
    size_t scnt, scap;
} Class;

static Class *arr = NULL;
static size_t cnt = 0, cap = 0;

static int grow(void) {
    if (cnt < cap) return 0;
    size_t nc = cap ? cap * 2 : 8;
    if (nc < cap) return -1;
    Class *t = realloc(arr, nc * sizeof(Class));
    if (!t) return -1;
    arr = t; cap = nc;
    return 0;
}

static int stu_grow(Class *c) {
    if (c->scnt < c->scap) return 0;
    size_t nc = c->scap ? c->scap * 2 : 4;
    if (nc < c->scap) return -1;
    Student *t = realloc(c->students, nc * sizeof(Student));
    if (!t) return -1;
    c->students = t; c->scap = nc;
    return 0;
}

static int find_class(const char *name) {
    for (size_t i = 0; i < cnt; i++)
        if (strcmp(arr[i].name, name) == 0) return (int)i;
    return -1;
}

static int find_student(Class *c, const char *name) {
    for (size_t i = 0; i < c->scnt; i++)
        if (strcmp(c->students[i].name, name) == 0) return (int)i;
    return -1;
}

static void free_class(Class *c) {
    free(c->name);
    for (size_t i = 0; i < c->scnt; i++) { free(c->students[i].name); free(c->students[i].note); }
    free(c->students);
    c->name = NULL; c->students = NULL; c->scnt = 0; c->scap = 0;
}

static void cmd_class(char *name) {
    if (find_class(name) >= 0) return;
    if (grow() < 0) return;
    arr[cnt].name = safe_strdup(name);
    arr[cnt].students = NULL; arr[cnt].scnt = 0; arr[cnt].scap = 0;
    if (!arr[cnt].name) return;
    cnt++;
}

static void cmd_student(char *cls, char *stu, char *note) {
    int idx = find_class(cls);
    if (idx < 0) return;
    Class *c = &arr[idx];
    if (find_student(c, stu) >= 0) return;
    if (stu_grow(c) < 0) return;
    c->students[c->scnt].name = safe_strdup(stu);
    c->students[c->scnt].note = safe_strdup(note);
    if (!c->students[c->scnt].name || !c->students[c->scnt].note) {
        free(c->students[c->scnt].name); free(c->students[c->scnt].note);
        c->students[c->scnt].name = NULL; c->students[c->scnt].note = NULL;
        return;
    }
    c->scnt++;
}

static void cmd_transfer(char *from, char *to, char *stu) {
    int ifr = find_class(from);
    int ito = find_class(to);
    if (ifr < 0 || ito < 0 || ifr == ito) return;
    Class *cf = &arr[ifr];
    Class *ct = &arr[ito];
    int is = find_student(cf, stu);
    if (is < 0) return;
    if (stu_grow(ct) < 0) return;
    ct->students[ct->scnt] = cf->students[is];
    ct->scnt++;
    for (size_t i = (size_t)is; i + 1 < cf->scnt; i++) cf->students[i] = cf->students[i + 1];
    cf->scnt--;
}

static void cmd_drop(char *cls, char *stu) {
    int idx = find_class(cls);
    if (idx < 0) return;
    Class *c = &arr[idx];
    int is = find_student(c, stu);
    if (is < 0) return;
    free(c->students[is].name); free(c->students[is].note);
    for (size_t i = (size_t)is; i + 1 < c->scnt; i++) c->students[i] = c->students[i + 1];
    c->scnt--;
}

static void cmd_deleteclass(char *name) {
    int idx = find_class(name);
    if (idx < 0) return;
    free_class(&arr[idx]);
    for (size_t i = (size_t)idx; i + 1 < cnt; i++) arr[i] = arr[i + 1];
    cnt--;
}

static void cmd_print(void) {
    for (size_t i = 0; i < cnt; i++) {
        printf("%s\n", arr[i].name);
        for (size_t j = 0; j < arr[i].scnt; j++)
            printf("  %s %s\n", arr[i].students[j].name, arr[i].students[j].note);
    }
}

static void cleanup(void) {
    for (size_t i = 0; i < cnt; i++) free_class(&arr[i]);
    free(arr); arr = NULL; cnt = 0; cap = 0;
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64] = {0}; int n1 = 0;
        if (sscanf(line, "%63s%n", cmd, &n1) < 1) continue;
        char *rest = line + n1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "CLASS") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_class(name);
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cls[256] = {0}, stu[256] = {0}; int n2 = 0, n3 = 0;
            if (sscanf(rest, "%255s%n %255s%n", cls, &n2, stu, &n3) < 2) continue;
            char *note = rest + n3;
            while (*note == ' ') note++;
            cmd_student(cls, stu, note);
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256] = {0}, to[256] = {0}, stu[256] = {0};
            if (sscanf(rest, "%255s %255s %255s", from, to, stu) < 3) continue;
            cmd_transfer(from, to, stu);
        } else if (strcmp(cmd, "DROP") == 0) {
            char cls[256] = {0}, stu[256] = {0};
            if (sscanf(rest, "%255s %255s", cls, stu) < 2) continue;
            cmd_drop(cls, stu);
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[256] = {0};
            if (sscanf(rest, "%255s", name) < 1) continue;
            cmd_deleteclass(name);
        } else if (strcmp(cmd, "PRINT") == 0) {
            cmd_print();
        }
    }
    cleanup();
    return 0;
}