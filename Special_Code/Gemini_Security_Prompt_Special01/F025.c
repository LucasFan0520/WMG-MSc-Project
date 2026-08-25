// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *stud_name;
    char *note;
} ClassroomStudent;

typedef struct {
    char *name;
    ClassroomStudent *students;
    size_t stud_count;
    size_t stud_capacity;
} Classroom;

int main(void) {
    Classroom *classes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
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
                    Classroom *new_c = realloc(classes, capacity * sizeof(Classroom));
                    if (new_c) classes = new_c;
                }
                classes[count].name = mystrdup(name);
                classes[count].students = NULL;
                classes[count].stud_count = 0;
                classes[count].stud_capacity = 0;
                count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cls = p;
            while (*cls == ' ') cls++;
            char *cls_end = cls;
            while (*cls_end && *cls_end != ' ') cls_end++;
            char *stud = cls_end;
            if (*cls_end != '\0') {
                *cls_end = '\0';
                stud++;
            }
            while (*stud == ' ') stud++;
            char *stud_end = stud;
            while (*stud_end && *stud_end != ' ') stud_end++;
            char *note = stud_end;
            if (*stud_end != '\0') {
                *stud_end = '\0';
                note++;
            }
            while (*note == ' ') note++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cls) == 0) {
                    Classroom *c = &classes[i];
                    if (c->stud_count >= c->stud_capacity) {
                        c->stud_capacity = c->stud_capacity == 0 ? 4 : c->stud_capacity * 2;
                        ClassroomStudent *new_stud = realloc(c->students, c->stud_capacity * sizeof(ClassroomStudent));
                        if (new_stud) c->students = new_stud;
                    }
                    c->students[c->stud_count].stud_name = mystrdup(stud);
                    c->students[c->stud_count].note = mystrdup(note);
                    c->stud_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *fromC = p;
            while (*fromC == ' ') fromC++;
            char *fromC_end = fromC;
            while (*fromC_end && *fromC_end != ' ') fromC_end++;
            char *toC = fromC_end;
            if (*fromC_end != '\0') {
                *fromC_end = '\0';
                toC++;
            }
            while (*toC == ' ') toC++;
            char *toC_end = toC;
            while (*toC_end && *toC_end != ' ') toC_end++;
            char *stud = toC_end;
            if (*toC_end != '\0') {
                *toC_end = '\0';
                stud++;
            }
            while (*stud == ' ') stud++;
            char *stud_end = stud;
            while (*stud_end && *stud_end != ' ') stud_end++;
            *stud_end = '\0';
            int f_idx = -1, t_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, fromC) == 0) f_idx = (int)i;
                if (strcmp(classes[i].name, toC) == 0) t_idx = (int)i;
            }
            if (f_idx != -1 && t_idx != -1) {
                Classroom *fc = &classes[f_idx];
                Classroom *tc = &classes[t_idx];
                for (size_t j = 0; j < fc->stud_count; j++) {
                    if (strcmp(fc->students[j].stud_name, stud) == 0) {
                        if (tc->stud_count >= tc->stud_capacity) {
                            tc->stud_capacity = tc->stud_capacity == 0 ? 4 : tc->stud_capacity * 2;
                            ClassroomStudent *new_stud = realloc(tc->students, tc->stud_capacity * sizeof(ClassroomStudent));
                            if (new_stud) tc->students = new_stud;
                        }
                        tc->students[tc->stud_count] = fc->students[j];
                        tc->stud_count++;
                        for (size_t k = j; k < fc->stud_count - 1; k++) {
                            fc->students[k] = fc->students[k + 1];
                        }
                        fc->stud_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *cls = p;
            while (*cls == ' ') cls++;
            char *cls_end = cls;
            while (*cls_end && *cls_end != ' ') cls_end++;
            char *stud = cls_end;
            if (*cls_end != '\0') {
                *cls_end = '\0';
                stud++;
            }
            while (*stud == ' ') stud++;
            char *stud_end = stud;
            while (*stud_end && *stud_end != ' ') stud_end++;
            *stud_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cls) == 0) {
                    Classroom *c = &classes[i];
                    for (size_t j = 0; j < c->stud_count; j++) {
                        if (strcmp(c->students[j].stud_name, stud) == 0) {
                            free(c->students[j].stud_name);
                            free(c->students[j].note);
                            for (size_t k = j; k < c->stud_count - 1; k++) {
                                c->students[k] = c->students[k + 1];
                            }
                            c->stud_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    free(classes[i].name);
                    for (size_t j = 0; j < classes[i].stud_count; j++) {
                        free(classes[i].students[j].stud_name);
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
                printf("CLASS %s\n", classes[i].name);
                for (size_t j = 0; j < classes[i].stud_count; j++) {
                    printf("STUDENT %s %s\n", classes[i].students[j].stud_name, classes[i].students[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(classes[i].name);
        for (size_t j = 0; j < classes[i].stud_count; j++) {
            free(classes[i].students[j].stud_name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
    }
    free(classes);
    return 0;
}
