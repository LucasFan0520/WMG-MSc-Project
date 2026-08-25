// F008.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

int main(void) {
    Student *gradebook = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *scr_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;

            if (*name == '\0' || *scr_str == '\0') continue;
            int score = (int)strtol(scr_str, NULL, 10);

            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(gradebook[i].name, name) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }

            if (strcmp(cmd, "ADD") == 0) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Student *new_gb = realloc(gradebook, new_cap * sizeof(Student));
                    if (!new_gb) continue;
                    gradebook = new_gb;
                    capacity = new_cap;
                }
                char *n = strdup(name);
                char *nt = strdup(note);
                if (n && nt) {
                    gradebook[count].name = n;
                    gradebook[count].score = score;
                    gradebook[count].note = nt;
                    count++;
                } else {
                    free(n);
                    free(nt);
                }
            } else {
                if (found_idx != -1) {
                    char *nt = strdup(note);
                    if (nt) {
                        free(gradebook[found_idx].note);
                        gradebook[found_idx].note = nt;
                        gradebook[found_idx].score = score;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(gradebook[i].name, name) == 0) {
                    free(gradebook[i].name);
                    free(gradebook[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        gradebook[j] = gradebook[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long long total = 0;
                for (size_t i = 0; i < count; i++) {
                    total += gradebook[i].score;
                }
                printf("%lld\n", total / (long long)count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d %s\n", gradebook[i].name, gradebook[i].score, gradebook[i].note);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(gradebook[i].name);
        free(gradebook[i].note);
    }
    free(gradebook);
    free(line);
    return 0;
}
