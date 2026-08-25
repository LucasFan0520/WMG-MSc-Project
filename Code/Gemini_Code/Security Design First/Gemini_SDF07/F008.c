/* F008.c */
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
    int capacity = 0;
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
            char *space1 = strchr(args, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *name = args;
            char *rest = space1 + 1;
            char *space2 = strchr(rest, ' ');
            int score = 0;
            char *note = "";
            if (space2) {
                *space2 = '\0';
                score = atoi(rest);
                note = space2 + 1;
            } else {
                score = atoi(rest);
            }
            char *n_name = strdup(name);
            char *n_note = strdup(note);
            if (n_name && n_note) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Student *tmp = realloc(book, new_cap * sizeof(Student));
                    if (tmp) {
                        book = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_name);
                        free(n_note);
                        *space1 = ' ';
                        if (space2) *space2 = ' ';
                        continue;
                    }
                }
                book[count].name = n_name;
                book[count].score = score;
                book[count].note = n_note;
                count++;
            } else {
                free(n_name);
                free(n_note);
            }
            *space1 = ' ';
            if (space2) *space2 = ' ';
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *args = line + 7;
            char *space1 = strchr(args, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *name = args;
            char *rest = space1 + 1;
            char *space2 = strchr(rest, ' ');
            int score = 0;
            char *note = "";
            if (space2) {
                *space2 = '\0';
                score = atoi(rest);
                note = space2 + 1;
            } else {
                score = atoi(rest);
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(book[i].name, name) == 0) {
                    char *n_note = strdup(note);
                    if (n_note) {
                        free(book[i].note);
                        book[i].note = n_note;
                        book[i].score = score;
                    }
                    break;
                }
            }
            *space1 = ' ';
            if (space2) *space2 = ' ';
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(book[i].name, name) == 0) {
                    free(book[i].name);
                    free(book[i].note);
                    for (int j = i; j < count - 1; j++) {
                        book[j] = book[j + 1];
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
                for (int i = 0; i < count; i++) {
                    sum += book[i].score;
                }
                printf("%lld\n", sum / count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", book[i].name, book[i].score, book[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(book[i].name);
        free(book[i].note);
    }
    free(book);
    return 0;
}
