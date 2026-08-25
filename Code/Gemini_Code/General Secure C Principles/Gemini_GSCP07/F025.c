// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *student_name;
    char *note;
} StudentRec;

typedef struct {
    char *name;
    StudentRec *students;
    size_t student_count;
    size_t student_capacity;
} ClassRec;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
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

int main(void) {
    ClassRec *classes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
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
            if (*p) *p = '\0';
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    ClassRec *nc = realloc(classes, capacity * sizeof(ClassRec));
                    if (!nc) break;
                    classes = nc;
                }
                classes[count].name = strdup(name);
                classes[count].students = NULL;
                classes[count].student_count = 0;
                classes[count].student_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            while (*p == ' ') p++;
            char *cname = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *sname = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    ClassRec *c = &classes[i];
                    if (c->student_count >= c->student_capacity) {
                        c->student_capacity = c->student_capacity == 0 ? 4 : c->student_capacity * 2;
                        StudentRec *ns = realloc(c->students, c->student_capacity * sizeof(StudentRec));
                        if (!ns) break;
                        c->students = ns;
                    }
                    c->students[c->student_count].student_name = strdup(sname);
                    c->students[c->student_count].note = strdup(note);
                    c->student_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            while (*p == ' ') p++;
            char *fromC = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *toC = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *sname = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int f_idx = -1, t_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, fromC) == 0) f_idx = (int)i;
                if (strcmp(classes[i].name, toC) == 0) t_idx = (int)i;
            }
            if (f_idx != -1 && t_idx != -1) {
                ClassRec *fc = &classes[f_idx];
                ClassRec *tc = &classes[t_idx];
                for (size_t j = 0; j < fc->student_count; j++) {
                    if (strcmp(fc->students[j].student_name, sname) == 0) {
                        if (tc->student_count >= tc->student_capacity) {
                            tc->student_capacity = tc->student_capacity == 0 ? 4 : tc->student_capacity * 2;
                            StudentRec *ns = realloc(tc->students, tc->student_capacity * sizeof(StudentRec));
                            if (!ns) break;
                            tc->students = ns;
                        }
                        tc->students[tc->student_count] = fc->students[j];
                        tc->student_count++;
                        for (size_t k = j; k < fc->student_count - 1; k++) {
                            fc->students[k] = fc->students[k + 1];
                        }
                        fc->student_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ') p++;
            char *cname = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *sname = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    ClassRec *c = &classes[i];
                    for (size_t j = 0; j < c->student_count; j++) {
                        if (strcmp(c->students[j].student_name, sname) == 0) {
                            free(c->students[j].student_name);
                            free(c->students[j].note);
                            for (size_t k = j; k < c->student_count - 1; k++) {
                                c->students[k] = c->students[k + 1];
                            }
                            c->student_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    free(classes[i].name);
                    for (size_t j = 0; j < classes[i].student_count; j++) {
                        free(classes[i].students[j].student_name);
                        free(classes[i].students[j].note);
                    }
                    free(classes[i].students);
                    for (size_t j = i; j < count - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("Class: %s\n", classes[i].name);
                for (size_t j = 0; j < classes[i].student_count; j++) {
                    printf("- %s: %s\n", classes[i].students[j].student_name, classes[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(classes[i].name);
        for (size_t j = 0; j < classes[i].student_count; j++) {
            free(classes[i].students[j].student_name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
