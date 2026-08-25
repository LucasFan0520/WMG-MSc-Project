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
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
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
    Class *roster = NULL;
    size_t c_count = 0;
    size_t c_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name = line + 6;
            if (c_count >= c_cap) {
                c_cap = c_cap == 0 ? 4 : c_cap * 2;
                Class *nb = realloc(roster, c_cap * sizeof(Class));
                if (!nb) {
                    free(line);
                    break;
                }
                roster = nb;
            }
            roster[c_count].name = safe_dup(name);
            roster[c_count].students = NULL;
            roster[c_count].s_count = 0;
            roster[c_count].s_cap = 0;
            c_count++;
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *p = line + 8;
            char *cls_name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *stud_name = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(roster[i].name, cls_name) == 0) {
                    Class *c = &roster[i];
                    if (c->s_count >= c->s_cap) {
                        c->s_cap = c->s_cap == 0 ? 4 : c->s_cap * 2;
                        Student *nb = realloc(c->students, c->s_cap * sizeof(Student));
                        if (!nb) break;
                        c->students = nb;
                    }
                    c->students[c->s_count].name = safe_dup(stud_name);
                    c->students[c->s_count].note = safe_dup(note);
                    c->s_count++;
                    break;
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *p = line + 9;
            char *from = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *to = p;
            while (*p && *p != ' ') p++;
            char *stud_name = "";
            if (*p == ' ') {
                *p = '\0';
                stud_name = p + 1;
            }
            Class *c_from = NULL, *c_to = NULL;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(roster[i].name, from) == 0) c_from = &roster[i];
                if (strcmp(roster[i].name, to) == 0) c_to = &roster[i];
            }
            if (c_from && c_to) {
                for (size_t j = 0; j < c_from->s_count; j++) {
                    if (strcmp(c_from->students[j].name, stud_name) == 0) {
                        if (c_to->s_count >= c_to->s_cap) {
                            c_to->s_cap = c_to->s_cap == 0 ? 4 : c_to->s_cap * 2;
                            Student *nb = realloc(c_to->students, c_to->s_cap * sizeof(Student));
                            if (!nb) break;
                            c_to->students = nb;
                        }
                        c_to->students[c_to->s_count] = c_from->students[j];
                        c_to->s_count++;
                        for (size_t k = j; k < c_from->s_count - 1; k++) {
                            c_from->students[k] = c_from->students[k + 1];
                        }
                        c_from->s_count--;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *p = line + 5;
            char *cls_name = p;
            while (*p && *p != ' ') p++;
            char *stud_name = "";
            if (*p == ' ') {
                *p = '\0';
                stud_name = p + 1;
            }
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(roster[i].name, cls_name) == 0) {
                    Class *c = &roster[i];
                    for (size_t j = 0; j < c->s_count; j++) {
                        if (strcmp(c->students[j].name, stud_name) == 0) {
                            free(c->students[j].name);
                            free(c->students[j].note);
                            for (size_t k = j; k < c->s_count - 1; k++) {
                                c->students[k] = c->students[k + 1];
                            }
                            c->s_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *cls_name = line + 12;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(roster[i].name, cls_name) == 0) {
                    free(roster[i].name);
                    for (size_t j = 0; j < roster[i].s_count; j++) {
                        free(roster[i].students[j].name);
                        free(roster[i].students[j].note);
                    }
                    free(roster[i].students);
                    for (size_t j = i; j < c_count - 1; j++) {
                        roster[j] = roster[j + 1];
                    }
                    c_count--;
                    break;
                }
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("Class: %s\n", roster[i].name);
                for (size_t j = 0; j < roster[i].s_count; j++) {
                    printf("  Student: %s - %s\n", roster[i].students[j].name, roster[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < c_count; i++) {
        free(roster[i].name);
        for (size_t j = 0; j < roster[i].s_count; j++) {
            free(roster[i].students[j].name);
            free(roster[i].students[j].note);
        }
        free(roster[i].students);
    }
    free(roster);
    return 0;
}
