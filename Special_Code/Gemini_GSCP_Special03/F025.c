// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(void) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            size_t ncap = cap * 2;
            char *nbuf = realloc(buf, ncap);
            if (!nbuf) {
                free(buf);
                return NULL;
            }
            buf = nbuf;
            cap = ncap;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *student_name;
    char *note;
} StudentInfo;

typedef struct {
    char *name;
    StudentInfo *students;
    size_t scount;
    size_t scap;
} ClassInfo;

int main(void) {
    ClassInfo *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line();
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
            while (*p == ' ') p++;
        }
        size_t clen = strlen(cmd);
        while (clen > 0 && (cmd[clen-1] == '\n' || cmd[clen-1] == '\n')) {
            cmd[clen-1] = '\0';
            clen--;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            if (count >= cap) {
                cap = cap == 0 ? 8 : cap * 2;
                ClassInfo *nitems = realloc(items, cap * sizeof(ClassInfo));
                if (nitems) items = nitems;
            }
            items[count].name = mystrdup(name);
            items[count].students = NULL;
            items[count].scount = 0;
            items[count].scap = 0;
            count++;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cls = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *stud = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *note = p;
            size_t ntlen = strlen(note);
            while (ntlen > 0 && (note[ntlen-1] == '\n' || note[ntlen-1] == '\n')) {
                note[ntlen-1] = '\0';
                ntlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, cls) == 0) {
                    ClassInfo *c = &items[i];
                    if (c->scount >= c->scap) {
                        c->scap = c->scap == 0 ? 4 : c->scap * 2;
                        StudentInfo *ns = realloc(c->students, c->scap * sizeof(StudentInfo));
                        if (ns) c->students = ns;
                    }
                    c->students[c->scount].student_name = mystrdup(stud);
                    c->students[c->scount].note = mystrdup(note);
                    c->scount++;
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *fromClass = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *toClass = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *stud = p;
            size_t stlen = strlen(stud);
            while (stlen > 0 && (stud[stlen-1] == '\n' || stud[stlen-1] == '\n')) {
                stud[stlen-1] = '\0';
                stlen--;
            }
            int fidx = -1, tidx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, fromClass) == 0) fidx = (int)i;
                if (strcmp(items[i].name, toClass) == 0) tidx = (int)i;
            }
            if (fidx != -1 && tidx != -1) {
                ClassInfo *fc = &items[fidx];
                ClassInfo *tc = &items[tidx];
                for (size_t j = 0; j < fc->scount; j++) {
                    if (strcmp(fc->students[j].student_name, stud) == 0) {
                        if (tc->scount >= tc->scap) {
                            tc->scap = tc->scap == 0 ? 4 : tc->scap * 2;
                            StudentInfo *ns = realloc(tc->students, tc->scap * sizeof(StudentInfo));
                            if (ns) tc->students = ns;
                        }
                        tc->students[tc->scount] = fc->students[j];
                        tc->scount++;
                        for (size_t k = j; k < fc->scount - 1; k++) {
                            fc->students[k] = fc->students[k+1];
                        }
                        fc->scount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *cls = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            char *stud = p;
            size_t stlen = strlen(stud);
            while (stlen > 0 && (stud[stlen-1] == '\n' || stud[stlen-1] == '\n')) {
                stud[stlen-1] = '\0';
                stlen--;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, cls) == 0) {
                    ClassInfo *c = &items[i];
                    for (size_t j = 0; j < c->scount; j++) {
                        if (strcmp(c->students[j].student_name, stud) == 0) {
                            free(c->students[j].student_name);
                            free(c->students[j].note);
                            for (size_t k = j; k < c->scount - 1; k++) {
                                c->students[k] = c->students[k+1];
                            }
                            c->scount--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = p;
            size_t nlen = strlen(name);
            while (nlen > 0 && (name[nlen-1] == '\n' || name[nlen-1] == '\n')) {
                name[nlen-1] = '\0';
                nlen--;
            }
            size_t i = 0;
            while (i < count) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    for (size_t j = 0; j < items[i].scount; j++) {
                        free(items[i].students[j].student_name);
                        free(items[i].students[j].note);
                    }
                    free(items[i].students);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j+1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Class: %s\n", items[i].name);
                for (size_t j = 0; j < items[i].scount; j++) {
                    printf("  Student: %s Note: %s\n", items[i].students[j].student_name, items[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        for (size_t j = 0; j < items[i].scount; j++) {
            free(items[i].students[j].student_name);
            free(items[i].students[j].note);
        }
        free(items[i].students);
    }
    free(items);
    return 0;
}
