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
    size_t s_count;
    size_t s_capacity;
} Class;

int main(void) {
    Class *classes = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "CLASS") == 0) {
            char *name = p;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Class *temp = realloc(classes, new_cap * sizeof(Class));
                    if (!temp) break;
                    classes = temp;
                    capacity = new_cap;
                }
                classes[count].name = strdup(name);
                classes[count].students = NULL;
                classes[count].s_count = 0;
                classes[count].s_capacity = 0;
                if (classes[count].name) count++;
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *sname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    Class *c = &classes[i];
                    if (c->s_count >= c->s_capacity) {
                        size_t new_cap = c->s_capacity == 0 ? 4 : c->s_capacity * 2;
                        Student *temp = realloc(c->students, new_cap * sizeof(Student));
                        if (!temp) break;
                        c->students = temp;
                        c->s_capacity = new_cap;
                    }
                    c->students[c->s_count].name = strdup(sname);
                    c->students[c->s_count].note = strdup(note);
                    if (c->students[c->s_count].name && c->students[c->s_count].note) {
                        c->s_count++;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *fromClass = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *toClass = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *sname = p;

            Class *fc = NULL;
            Class *tc = NULL;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, fromClass) == 0) fc = &classes[i];
                if (strcmp(classes[i].name, toClass) == 0) tc = &classes[i];
            }
            if (fc && tc) {
                for (size_t j = 0; j < fc->s_count; j++) {
                    if (strcmp(fc->students[j].name, sname) == 0) {
                        if (tc->s_count >= tc->s_capacity) {
                            size_t new_cap = tc->s_capacity == 0 ? 4 : tc->s_capacity * 2;
                            Student *temp = realloc(tc->students, new_cap * sizeof(Student));
                            if (!temp) break;
                            tc->students = temp;
                            tc->s_capacity = new_cap;
                        }
                        tc->students[tc->s_count] = fc->students[j];
                        tc->s_count++;
                        for (size_t k = j; k < fc->s_count - 1; k++) {
                            fc->students[k] = fc->students[k + 1];
                        }
                        fc->s_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *cname = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *sname = p;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    Class *c = &classes[i];
                    for (size_t j = 0; j < c->s_count; j++) {
                        if (strcmp(c->students[j].name, sname) == 0) {
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
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *name = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(classes[i].name, name) == 0) {
                    free(classes[i].name);
                    for (size_t j = 0; j < classes[i].s_count; j++) {
                        free(classes[i].students[j].name);
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
                for (size_t j = 0; j < classes[i].s_count; j++) {
                    printf("STUDENT %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
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
