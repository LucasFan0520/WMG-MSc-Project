// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

int main() {
    Student *book = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *name = strtok(NULL, " ");
            char *score_s = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && score_s && note) {
                book = realloc(book, (count + 1) * sizeof(Student));
                book[count].name = strdup(name);
                book[count].score = atoi(score_s);
                book[count].note = strdup(note);
                count++;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name = strtok(NULL, " ");
            char *score_s = strtok(NULL, " ");
            char *note = strtok(NULL, "");
            if (name && score_s && note) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(book[i].name, name) == 0) {
                        book[i].score = atoi(score_s);
                        free(book[i].note);
                        book[i].note = strdup(note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(book[i].name, name) == 0) {
                        free(book[i].name);
                        free(book[i].note);
                        for (int j = i; j < count - 1; j++) {
                            book[j] = book[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(book);
                    book = NULL;
                } else {
                    book = realloc(book, count * sizeof(Student));
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                int sum = 0;
                for (int i = 0; i < count; i++) {
                    sum += book[i].score;
                }
                printf("%d\n", sum / count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
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
    return 0;
}
