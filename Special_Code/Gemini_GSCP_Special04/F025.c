/* F025.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
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
    char *name;
    char *note;
} Student;

typedef struct {
    char *name;
    Student *students;
    size_t s_count;
    size_t s_cap;
} Class;

int main(void) {
    Class *classes = NULL;
    size_t c_count = 0;
    size_t c_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "CLASS") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found == -1) {
                if (c_count >= c_cap) {
                    c_cap = c_cap == 0 ? 4 : c_cap * 2;
                    Class *nc = realloc(classes, c_cap * sizeof(Class));
                    if (nc) classes = nc;
                }
                char *nn = mystrdup(name);
                if (nn) {
                    classes[c_count].name = nn;
                    classes[c_count].students = NULL;
                    classes[c_count].s_count = 0;
                    classes[c_count].s_cap = 0;
                    c_count++;
                }
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            while (*p == ' ') p++;
            char *cls = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *student = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            int found = -1;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, cls) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                Class *cl = &classes[found];
                if (cl->s_count >= cl->s_cap) {
                    cl->s_cap = cl->s_cap == 0 ? 4 : cl->s_cap * 2;
                    Student *ns = realloc(cl->students, cl->s_cap * sizeof(Student));
                    if (ns) cl->students = ns;
                }
                char *sname = mystrdup(student);
                char *snote = mystrdup(note);
                if (sname && snote) {
                    cl->students[cl->s_count].name = sname;
                    cl->students[cl->s_count].note = snote;
                    cl->s_count++;
                } else {
                    free(sname); free(snote);
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            while (*p == ' ') p++;
            char *fromC = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *toC = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *student = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int idxFrom = -1, idxTo = -1;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, fromC) == 0) idxFrom = (int)i;
                if (strcmp(classes[i].name, toC) == 0) idxTo = (int)i;
            }
            if (idxFrom != -1 && idxTo != -1) {
                Class *cf = &classes[idxFrom];
                Class *ct = &classes[idxTo];
                int s_idx = -1;
                for (size_t j = 0; j < cf->s_count; j++) {
                    if (strcmp(cf->students[j].name, student) == 0) {
                        s_idx = (int)j;
                        break;
                    }
                }
                if (s_idx != -1) {
                    if (ct->s_count >= ct->s_cap) {
                        ct->s_cap = ct->s_cap == 0 ? 4 : ct->s_cap * 2;
                        Student *ns = realloc(ct->students, ct->s_cap * sizeof(Student));
                        if (ns) ct->students = ns;
                    }
                    ct->students[ct->s_count] = cf->students[s_idx];
                    ct->s_count++;
                    for (size_t j = (size_t)s_idx; j < cf->s_count - 1; j++) {
                        cf->students[j] = cf->students[j + 1];
                    }
                    cf->s_count--;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ') p++;
            char *cls = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *student = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, cls) == 0) {
                    Class *cl = &classes[i];
                    int s_idx = -1;
                    for (size_t j = 0; j < cl->s_count; j++) {
                        if (strcmp(cl->students[j].name, student) == 0) {
                            s_idx = (int)j;
                            break;
                        }
                    }
                    if (s_idx != -1) {
                        free(cl->students[s_idx].name);
                        free(cl->students[s_idx].note);
                        for (size_t j = (size_t)s_idx; j < cl->s_count - 1; j++) {
                            cl->students[j] = cl->students[j + 1];
                        }
                        cl->s_count--;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(classes[found].name);
                for (size_t j = 0; j < classes[found].s_count; j++) {
                    free(classes[found].students[j].name);
                    free(classes[found].students[j].note);
                }
                free(classes[found].students);
                for (size_t i = (size_t)found; i < c_count - 1; i++) {
                    classes[i] = classes[i + 1];
                }
                c_count--;
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("CLASS: %s\n", classes[i].name);
                for (size_t j = 0; j < classes[i].s_count; j++) {
                    printf("  STUDENT: %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < c_count; i++) {
        free(classes[i].name);
        for (size_t j = 0; j < classes[i].s_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
