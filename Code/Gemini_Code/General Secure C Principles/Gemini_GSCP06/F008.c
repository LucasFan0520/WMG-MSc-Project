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
    Student *gb = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *name = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    int score = atoi(rest);
                    char *note = sp2 + 1;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Student *next = realloc(gb, capacity * sizeof(Student));
                        if (!next) break;
                        gb = next;
                    }
                    gb[count].name = strdup(name);
                    gb[count].score = score;
                    gb[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *args = line + 7;
            char *sp1 = strchr(args, ' ');
            if (sp1) {
                *sp1 = '\0';
                char *name = args;
                char *rest = sp1 + 1;
                char *sp2 = strchr(rest, ' ');
                if (sp2) {
                    *sp2 = '\0';
                    int score = atoi(rest);
                    char *note = sp2 + 1;
                    for (size_t i = 0; i < count; i++) {
                        if (strcmp(gb[i].name, name) == 0) {
                            gb[i].score = score;
                            free(gb[i].note);
                            gb[i].note = strdup(note);
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(gb[i].name, name) == 0) {
                    free(gb[i].name);
                    free(gb[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        gb[j] = gb[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long long sum = 0;
                for (size_t i = 0; i < count; i++) {
                    sum += gb[i].score;
                }
                printf("%lld\n", sum / (long long)count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d %s\n", gb[i].name, gb[i].score, gb[i].note);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(gb[i].name);
        free(gb[i].note);
    }
    free(gb);
    return 0;
}
