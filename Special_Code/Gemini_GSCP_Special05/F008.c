// F008.c
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
    int score;
    char *note;
} Student;

int main(void) {
    Student *gradebook = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *score_str = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            int sc = (int)strtol(score_str, NULL, 10);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Student *nb = realloc(gradebook, cap * sizeof(Student));
                if (!nb) {
                    free(line);
                    break;
                }
                gradebook = nb;
            }
            gradebook[count].name = safe_dup(name);
            gradebook[count].score = sc;
            gradebook[count].note = safe_dup(note);
            count++;
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *p = line + 7;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *score_str = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            int sc = (int)strtol(score_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(gradebook[i].name, name) == 0) {
                    gradebook[i].score = sc;
                    free(gradebook[i].note);
                    gradebook[i].note = safe_dup(note);
                    break;
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name = line + 5;
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
        } else if (strcmp(line, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long long total = 0;
                for (size_t i = 0; i < count; i++) {
                    total += gradebook[i].score;
                }
                printf("%lld\n", total / (long long)count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d %s\n", gradebook[i].name, gradebook[i].score, gradebook[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(gradebook[i].name);
        free(gradebook[i].note);
    }
    free(gradebook);
    return 0;
}
