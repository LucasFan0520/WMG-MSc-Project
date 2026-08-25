// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
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

typedef struct {
    char *student;
    char *note;
} Student;

typedef struct {
    char *name;
    Student *students;
    size_t scount;
    size_t scap;
} Class;

int main() {
    Class *classes = NULL;
    size_t ccount = 0;
    size_t ccap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name = line + 6;
            if (ccount >= ccap) {
                ccap = ccap == 0 ? 4 : ccap * 2;
                classes = realloc(classes, ccap * sizeof(Class));
            }
            classes[ccount].name = strdup(name);
            classes[ccount].students = NULL;
            classes[ccount].scount = 0;
            classes[ccount].scap = 0;
            ccount++;
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *p = line + 8;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *cls_name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                char *student = p2;
                char *note = "";
                if (s2) {
                    *s2 = '\0';
                    note = s2 + 1;
                }
                for (size_t i = 0; i < ccount; i++) {
                    if (strcmp(classes[i].name, cls_name) == 0) {
                        Class *c = &classes[i];
                        if (c->scount >= c->scap) {
                            c->scap = c->scap == 0 ? 4 : c->scap * 2;
                            c->students = realloc(c->students, c->scap * sizeof(Student));
                        }
                        c->students[c->scount].student = strdup(student);
                        c->students[c->scount].note = strdup(note);
                        c->scount++;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *p = line + 9;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *from_cls = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *to_cls = p2;
                    char *student = s2 + 1;
                    int f_idx = -1, t_idx = -1;
                    for (size_t i = 0; i < ccount; i++) {
                        if (strcmp(classes[i].name, from_cls) == 0) f_idx = (int)i;
                        if (strcmp(classes[i].name, to_cls) == 0) t_idx = (int)i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        Class *fc = &classes[f_idx];
                        Class *tc = &classes[t_idx];
                        for (size_t j = 0; j < fc->scount; j++) {
                            if (strcmp(fc->students[j].student, student) == 0) {
                                if (tc->scount >= tc->scap) {
                                    tc->scap = tc->scap == 0 ? 4 : tc->scap * 2;
                                    tc->students = realloc(tc->students, tc->scap * sizeof(Student));
                                }
                                tc->students[tc->scount] = fc->students[j];
                                tc->scount++;
                                for (size_t k = j; k < fc->scount - 1; k++) {
                                    fc->students[k] = fc->students[k + 1];
                                }
                                fc->scount--;
                                break;
                            }
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *p = line + 5;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *cls_name = p;
                char *student = s1 + 1;
                for (size_t i = 0; i < ccount; i++) {
                    if (strcmp(classes[i].name, cls_name) == 0) {
                        Class *c = &classes[i];
                        for (size_t j = 0; j < c->scount; j++) {
                            if (strcmp(c->students[j].student, student) == 0) {
                                free(c->students[j].student);
                                free(c->students[j].note);
                                for (size_t k = j; k < c->scount - 1; k++) {
                                    c->students[k] = c->students[k + 1];
                                }
                                c->scount--;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *name = line + 12;
            for (size_t i = 0; i < ccount; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    free(classes[i].name);
                    for (size_t j = 0; j < classes[i].scount; j++) {
                        free(classes[i].students[j].student);
                        free(classes[i].students[j].note);
                    }
                    free(classes[i].students);
                    for (size_t j = i; j < ccount - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    ccount--;
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < ccount; i++) {
                printf("Class %s:\n", classes[i].name);
                for (size_t j = 0; j < classes[i].scount; j++) {
                    printf("- %s: %s\n", classes[i].students[j].student, classes[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < ccount; i++) {
        free(classes[i].name);
        for (size_t j = 0; j < classes[i].scount; j++) {
            free(classes[i].students[j].student);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
