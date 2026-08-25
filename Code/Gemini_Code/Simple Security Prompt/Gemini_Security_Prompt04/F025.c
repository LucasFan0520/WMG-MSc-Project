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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
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
} ClassInfo;

int main(void) {
    ClassInfo *classes = NULL;
    int c_count = 0;
    int c_cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "CLASS ", 6) == 0) {
            char *name = line + 6;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found == -1) {
                if (c_count >= c_cap) {
                    c_cap = c_cap == 0 ? 16 : c_cap * 2;
                    classes = realloc(classes, c_cap * sizeof(ClassInfo));
                }
                classes[c_count].name = strdup(name);
                classes[c_count].students = NULL;
                classes[c_count].s_count = 0;
                classes[c_count].s_cap = 0;
                c_count++;
            }
        } else if (strncmp(line, "STUDENT ", 8) == 0) {
            char *p = line + 8;
            while (*p == ' ') p++;
            char *cls = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *stud = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    while (*note == ' ') note++;
                    for (int i = 0; i < c_count; i++) {
                        if (strcmp(classes[i].name, cls) == 0) {
                            ClassInfo *ci = &classes[i];
                            if (ci->s_count >= ci->s_cap) {
                                ci->s_cap = ci->s_cap == 0 ? 16 : ci->s_cap * 2;
                                ci->students = realloc(ci->students, ci->s_cap * sizeof(Student));
                            }
                            ci->students[ci->s_count].name = strdup(stud);
                            ci->students[ci->s_count].note = strdup(note);
                            ci->s_count++;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "TRANSFER ", 9) == 0) {
            char *p = line + 9;
            while (*p == ' ') p++;
            char *fromC = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *toC = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *stud = p + 1;
                    while (*stud == ' ') stud++;
                    int f_idx = -1, t_idx = -1;
                    for (int i = 0; i < c_count; i++) {
                        if (strcmp(classes[i].name, fromC) == 0) f_idx = i;
                        if (strcmp(classes[i].name, toC) == 0) t_idx = i;
                    }
                    if (f_idx != -1 && t_idx != -1) {
                        int s_match = -1;
                        ClassInfo *fc = &classes[f_idx];
                        for (int j = 0; j < fc->s_count; j++) {
                            if (strcmp(fc->students[j].name, stud) == 0) {
                                s_match = j;
                                break;
                            }
                        }
                        if (s_match != -1) {
                            ClassInfo *tc = &classes[t_idx];
                            if (tc->s_count >= tc->s_cap) {
                                tc->s_cap = tc->s_cap == 0 ? 16 : tc->s_cap * 2;
                                tc->students = realloc(tc->students, tc->s_cap * sizeof(Student));
                            }
                            tc->students[tc->s_count] = fc->students[s_match];
                            tc->s_count++;
                            for (int j = s_match; j < fc->s_count - 1; j++) {
                                fc->students[j] = fc->students[j + 1];
                            }
                            fc->s_count--;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *cls = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *stud = p + 1;
                while (*stud == ' ') stud++;
                for (int i = 0; i < c_count; i++) {
                    if (strcmp(classes[i].name, cls) == 0) {
                        ClassInfo *ci = &classes[i];
                        int idx = -1;
                        for (int j = 0; j < ci->s_count; j++) {
                            if (strcmp(ci->students[j].name, stud) == 0) {
                                idx = j;
                                break;
                            }
                        }
                        if (idx != -1) {
                            free(ci->students[idx].name);
                            free(ci->students[idx].note);
                            for (int j = idx; j < ci->s_count - 1; j++) {
                                ci->students[j] = ci->students[j + 1];
                            }
                            ci->s_count--;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DELETECLASS ", 12) == 0) {
            char *name = line + 12;
            while (*name == ' ') name++;
            int idx = -1;
            for (int i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(classes[idx].name);
                for (int j = 0; j < classes[idx].s_count; j++) {
                    free(classes[idx].students[j].name);
                    free(classes[idx].students[j].note);
                }
                free(classes[idx].students);
                for (int i = idx; i < c_count - 1; i++) {
                    classes[i] = classes[i + 1];
                }
                c_count--;
            }
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < c_count; i++) {
                printf("Class: %s\n", classes[i].name);
                for (int j = 0; j < classes[i].s_count; j++) {
                    printf("  Student: %s - %s\n", classes[i].students[j].name, classes[i].students[j].note);
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
