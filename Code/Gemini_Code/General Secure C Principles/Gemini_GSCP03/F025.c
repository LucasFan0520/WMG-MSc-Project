// F025.c
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *note;
} Student;

typedef struct {
    char *name;
    Student *students;
    int s_count;
    int s_cap;
} Classroom;

int main() {
    Classroom *classes = NULL;
    int c_count = 0;
    int c_cap = 0;
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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "PRINT") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("CLASS %s\n", classes[i].name);
                for (int j = 0; j < classes[i].s_count; j++) {
                    printf("%s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
            free(line);
            continue;
        }
        while (*p == ' ') p++;
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int found = 0;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (c_count >= c_cap) {
                    c_cap = c_cap == 0 ? 16 : c_cap * 2;
                    Classroom *nb = realloc(classes, c_cap * sizeof(Classroom));
                    if (nb) classes = nb;
                }
                classes[c_count].name = strdup(name);
                classes[c_count].students = NULL;
                classes[c_count].s_count = 0;
                classes[c_count].s_cap = 0;
                c_count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cls = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *std = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, cls) == 0) {
                    Classroom *cl = &classes[i];
                    if (cl->s_count >= cl->s_cap) {
                        cl->s_cap = cl->s_cap == 0 ? 16 : cl->s_cap * 2;
                        Student *nb = realloc(cl->students, cl->s_cap * sizeof(Student));
                        if (nb) cl->students = nb;
                    }
                    cl->students[cl->s_count].name = strdup(std);
                    cl->students[cl->s_count].note = strdup(note);
                    cl->s_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *fromClass = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *toClass = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *std = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int f_idx = -1, t_idx = -1;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, fromClass) == 0) f_idx = i;
                if (strcmp(classes[i].name, toClass) == 0) t_idx = i;
            }
            if (f_idx != -1 && t_idx != -1) {
                Classroom *fc = &classes[f_idx];
                Classroom *tc = &classes[t_idx];
                for (int j = 0; j < fc->s_count; j++) {
                    if (strcmp(fc->students[j].name, std) == 0) {
                        if (tc->s_count >= tc->s_cap) {
                            tc->s_cap = tc->s_cap == 0 ? 16 : tc->s_cap * 2;
                            Student *nb = realloc(tc->students, tc->s_cap * sizeof(Student));
                            if (nb) tc->students = nb;
                        }
                        tc->students[tc->s_count] = fc->students[j];
                        tc->s_count++;
                        for (int k = j; k < fc->s_count - 1; k++) {
                            fc->students[k] = fc->students[k + 1];
                        }
                        fc->s_count--;
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
            }
            while (*p == ' ') p++;
            char *std = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, cls) == 0) {
                    Classroom *cl = &classes[i];
                    for (int j = 0; j < cl->s_count; j++) {
                        if (strcmp(cl->students[j].name, std) == 0) {
                            free(cl->students[j].name);
                            free(cl->students[j].note);
                            for (int k = j; k < cl->s_count - 1; k++) {
                                cl->students[k] = cl->students[k + 1];
                            }
                            cl->s_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (int i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    free(classes[i].name);
                    for (int j = 0; j < classes[i].s_count; j++) {
                        free(classes[i].students[j].name);
                        free(classes[i].students[j].note);
                    }
                    free(classes[i].students);
                    for (int j = i; j < c_count - 1; j++) {
                        classes[j] = classes[j + 1];
                    }
                    c_count--;
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < c_count; i++) {
        free(classes[i].name);
        for (int j = 0; j < classes[i].s_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
