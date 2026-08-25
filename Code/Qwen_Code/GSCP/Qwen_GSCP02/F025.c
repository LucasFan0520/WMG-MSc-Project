/* F025.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *note;
} Student;

typedef struct {
    char *name;
    Student *students;
    int scount;
    int scap;
} Class;

static Class *classes = NULL;
static int ccount = 0;
static int ccap = 0;

static char *dup_str(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) memcpy(d, s, len + 1);
    return d;
}

static int find_class(const char *name) {
    for (int i = 0; i < ccount; i++)
        if (strcmp(classes[i].name, name) == 0) return i;
    return -1;
}

static int find_student(Class *c, const char *name) {
    for (int i = 0; i < c->scount; i++)
        if (strcmp(c->students[i].name, name) == 0) return i;
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[64] = {0};
        int offset = 0;
        if (sscanf(line, "%63s%n", cmd, &offset) != 1) continue;

        if (strcmp(cmd, "CLASS") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            if (find_class(name) >= 0) continue;
            if (ccount >= ccap) {
                ccap = ccap == 0 ? 8 : ccap * 2;
                classes = realloc(classes, sizeof(Class) * ccap);
            }
            classes[ccount].name = dup_str(name);
            classes[ccount].students = NULL;
            classes[ccount].scount = 0;
            classes[ccount].scap = 0;
            ccount++;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char cls[256] = {0}, stu[256] = {0};
            int off2 = 0, off3 = 0;
            if (sscanf(line + offset, "%255s%n %255s%n", cls, &off2, stu, &off3) != 2) continue;
            const char *note = line + offset + off2 + off3;
            while (*note == ' ') note++;
            int ci = find_class(cls);
            if (ci < 0) continue;
            Class *c = &classes[ci];
            if (c->scount >= c->scap) {
                c->scap = c->scap == 0 ? 4 : c->scap * 2;
                c->students = realloc(c->students, sizeof(Student) * c->scap);
            }
            c->students[c->scount].name = dup_str(stu);
            c->students[c->scount].note = dup_str(note);
            c->scount++;
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char from[256] = {0}, to[256] = {0}, stu[256] = {0};
            if (sscanf(line + offset, "%255s %255s %255s", from, to, stu) != 3) continue;
            int fi = find_class(from);
            int ti = find_class(to);
            if (fi < 0 || ti < 0) continue;
            Class *fc = &classes[fi];
            Class *tc = &classes[ti];
            int si = find_student(fc, stu);
            if (si < 0) continue;
            Student s = fc->students[si];
            for (int i = si; i < fc->scount - 1; i++) fc->students[i] = fc->students[i + 1];
            fc->scount--;
            if (tc->scount >= tc->scap) {
                tc->scap = tc->scap == 0 ? 4 : tc->scap * 2;
                tc->students = realloc(tc->students, sizeof(Student) * tc->scap);
            }
            tc->students[tc->scount++] = s;
        } else if (strcmp(cmd, "DROP") == 0) {
            char cls[256] = {0}, stu[256] = {0};
            if (sscanf(line + offset, "%255s %255s", cls, stu) != 2) continue;
            int ci = find_class(cls);
            if (ci < 0) continue;
            Class *c = &classes[ci];
            int si = find_student(c, stu);
            if (si < 0) continue;
            free(c->students[si].name);
            free(c->students[si].note);
            for (int i = si; i < c->scount - 1; i++) c->students[i] = c->students[i + 1];
            c->scount--;
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char name[256] = {0};
            if (sscanf(line + offset, "%255s", name) != 1) continue;
            int ci = find_class(name);
            if (ci < 0) continue;
            free(classes[ci].name);
            for (int i = 0; i < classes[ci].scount; i++) {
                free(classes[ci].students[i].name);
                free(classes[ci].students[i].note);
            }
            free(classes[ci].students);
            for (int i = ci; i < ccount - 1; i++) classes[i] = classes[i + 1];
            ccount--;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < ccount; i++) {
                printf("%s\n", classes[i].name);
                for (int j = 0; j < classes[i].scount; j++) {
                    printf("  %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    for (int i = 0; i < ccount; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].scount; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}