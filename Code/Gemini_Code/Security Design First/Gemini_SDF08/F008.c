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
    Student *book = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *score_str = p2;
                    char *note = s2 + 1;
                    int scr = atoi(score_str);
                    Student *tmp = realloc(book, sizeof(Student) * (count + 1));
                    if (tmp) {
                        book = tmp;
                        book[count].name = strdup(name);
                        book[count].score = scr;
                        book[count].note = strdup(note);
                        if (book[count].name && book[count].note) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *p = line + 7;
            char *s1 = strchr(p, ' ');
            if (s1) {
                *s1 = '\0';
                char *name = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *score_str = p2;
                    char *note = s2 + 1;
                    int scr = atoi(score_str);
                    for (int i = 0; i < count; i++) {
                        if (strcmp(book[i].name, name) == 0) {
                            char *nn = strdup(note);
                            if (nn) {
                                free(book[i].note);
                                book[i].note = nn;
                                book[i].score = scr;
                            }
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name = line + 5;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(book[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(book[found].name);
                free(book[found].note);
                for (int i = found; i < count - 1; i++) {
                    book[i] = book[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long long total = 0;
                for (int i = 0; i < count; i++) {
                    total += book[i].score;
                }
                printf("%lld\n", total / count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", book[i].name, book[i].score, book[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(book[i].name);
        free(book[i].note);
    }
    free(book);
    free(line);
    return 0;
}
