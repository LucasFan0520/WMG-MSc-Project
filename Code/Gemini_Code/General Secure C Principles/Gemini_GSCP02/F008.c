// F008.c
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
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char name[256];
            int score;
            char note[1024];
            if (sscanf(line + 4, "%255s %d %[^\n]", name, &score, note) == 3) {
                Student *tmp = realloc(book, (count + 1) * sizeof(Student));
                if (tmp) {
                    book = tmp;
                    book[count].name = strdup(name);
                    book[count].score = score;
                    book[count].note = strdup(note);
                    count++;
                }
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char name[256];
            int score;
            char note[1024];
            if (sscanf(line + 7, "%255s %d %[^\n]", name, &score, note) == 3) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(book[i].name, name) == 0) {
                        char *nn = strdup(note);
                        if (nn) {
                            free(book[i].note);
                            book[i].note = nn;
                            book[i].score = score;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') {
                name++;
            }
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
            if (count > 0) {
                long long total = 0;
                for (int i = 0; i < count; i++) {
                    total += book[i].score;
                }
                printf("%lld\n", total / count);
            } else {
                printf("0\n");
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
