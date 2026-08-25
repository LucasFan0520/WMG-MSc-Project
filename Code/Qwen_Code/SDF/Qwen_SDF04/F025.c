// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* name; char* note; } Student;
typedef struct { char* name; Student* students; size_t s_count, s_cap; } Class;

Class* classes = NULL;
size_t c_count = 0, c_cap = 0;

void cleanup() {
    for (size_t i = 0; i < c_count; i++) {
        for (size_t j = 0; j < classes[i].s_count; j++) {
            free(classes[i].students[j].name);
            free(classes[i].students[j].note);
        }
        free(classes[i].students);
        free(classes[i].name);
    }
    free(classes);
}

int main() {
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "CLASS") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            if (!*ptr) continue;
            if (c_count == c_cap) {
                size_t new_cap = c_cap == 0 ? 4 : c_cap * 2;
                Class* temp = realloc(classes, new_cap * sizeof(Class));
                if (!temp) { cleanup(); free(line); return 1; }
                classes = temp; c_cap = new_cap;
            }
            classes[c_count].name = strdup(ptr);
            classes[c_count].students = NULL;
            classes[c_count].s_count = 0;
            classes[c_count].s_cap = 0;
            if (!classes[c_count].name) { cleanup(); free(line); return 1; }
            c_count++;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* cname = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* sname = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;

            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    if (classes[i].s_count == classes[i].s_cap) {
                        size_t new_cap = classes[i].s_cap == 0 ? 4 : classes[i].s_cap * 2;
                        Student* temp = realloc(classes[i].students, new_cap * sizeof(Student));
                        if (!temp) { cleanup(); free(line); return 1; }
                        classes[i].students = temp; classes[i].s_cap = new_cap;
                    }
                    classes[i].students[classes[i].s_count].name = strdup(sname);
                    classes[i].students[classes[i].s_count].note = strdup(note);
                    if (!classes[i].students[classes[i].s_count].name || !classes[i].students[classes[i].s_count].note) {
                        cleanup(); free(line); return 1;
                    }
                    classes[i].s_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* from = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* to = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* sname = ptr;

            Class* cf = NULL, *ct = NULL;
            Student* s = NULL;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, from) == 0) cf = &classes[i];
                if (strcmp(classes[i].name, to) == 0) ct = &classes[i];
            }
            if (cf && ct) {
                for (size_t i = 0; i < cf->s_count; i++) {
                    if (strcmp(cf->students[i].name, sname) == 0) {
                        s = &cf->students[i];
                        break;
                    }
                }
                if (s) {
                    if (ct->s_count == ct->s_cap) {
                        size_t new_cap = ct->s_cap == 0 ? 4 : ct->s_cap * 2;
                        Student* temp = realloc(ct->students, new_cap * sizeof(Student));
                        if (!temp) { cleanup(); free(line); return 1; }
                        ct->students = temp; ct->s_cap = new_cap;
                    }
                    ct->students[ct->s_count] = *s;
                    ct->s_count++;
                    for (size_t i = 0; i < cf->s_count; i++) {
                        if (&cf->students[i] == s) {
                            for (size_t j = i; j < cf->s_count - 1; j++) cf->students[j] = cf->students[j + 1];
                            break;
                        }
                    }
                    cf->s_count--;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* cname = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* sname = ptr;

            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, cname) == 0) {
                    for (size_t j = 0; j < classes[i].s_count; j++) {
                        if (strcmp(classes[i].students[j].name, sname) == 0) {
                            free(classes[i].students[j].name);
                            free(classes[i].students[j].note);
                            for (size_t k = j; k < classes[i].s_count - 1; k++) classes[i].students[k] = classes[i].students[k + 1];
                            classes[i].s_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(classes[i].name, ptr) == 0) {
                    for (size_t j = 0; j < classes[i].s_count; j++) {
                        free(classes[i].students[j].name);
                        free(classes[i].students[j].note);
                    }
                    free(classes[i].students);
                    free(classes[i].name);
                    for (size_t j = i; j < c_count - 1; j++) classes[j] = classes[j + 1];
                    c_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s\n", classes[i].name);
                for (size_t j = 0; j < classes[i].s_count; j++) {
                    printf("  %s %s\n", classes[i].students[j].name, classes[i].students[j].note);
                }
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}