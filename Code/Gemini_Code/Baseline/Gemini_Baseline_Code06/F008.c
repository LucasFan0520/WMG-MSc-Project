// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
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

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

int main() {
    Student *book = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char name[128];
            int score;
            if (sscanf(line + 4, "%127s %d", name, &score) == 2) {
                char *p = line + 4;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                book = realloc(book, (count + 1) * sizeof(Student));
                book[count].name = strdup(name);
                book[count].score = score;
                book[count].note = strdup(p);
                count++;
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char name[128];
            int score;
            if (sscanf(line + 7, "%127s %d", name, &score) == 2) {
                char *p = line + 7;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                if (*p == ' ') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(book[i].name, name) == 0) {
                        book[i].score = score;
                        free(book[i].note);
                        book[i].note = strdup(p);
                        break;
                    }
                }
            }
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
                    book = realloc(book, count * sizeof(Student));
                    break;
                }
            }
        } else if (strcmp(line, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                int sum = 0;
                for (int i = 0; i < count; i++) {
                    sum += book[i].score;
                }
                printf("%d\n", sum / count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", book[i].name, book[i].score, book[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(book[i].name);
        free(book[i].note);
    }
    free(book);
    return 0;
}
