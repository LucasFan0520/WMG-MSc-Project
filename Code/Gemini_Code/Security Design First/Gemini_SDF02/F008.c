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
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *name_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *score_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note_str = p;
            int s_val = atoi(score_str);

            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(book[i].name, name_str) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Student *temp = realloc(book, new_cap * sizeof(Student));
                    if (!temp) break;
                    book = temp;
                    capacity = new_cap;
                }
                book[count].name = strdup(name_str);
                book[count].score = s_val;
                book[count].note = strdup(note_str);
                if (book[count].name && book[count].note) {
                    count++;
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *name_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *score_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note_str = p;
            int s_val = atoi(score_str);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(book[i].name, name_str) == 0) {
                    char *temp_note = strdup(note_str);
                    if (temp_note) {
                        free(book[i].note);
                        book[i].note = temp_note;
                        book[i].score = s_val;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name_str = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(book[i].name, name_str) == 0) {
                    free(book[i].name);
                    free(book[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        book[j] = book[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (count == 0) {
                printf("0\n");
            } else {
                long long sum = 0;
                for (size_t i = 0; i < count; i++) {
                    sum += book[i].score;
                }
                printf("%lld\n", sum / (long long)count);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d %s\n", book[i].name, book[i].score, book[i].note);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(book[i].name);
        free(book[i].note);
    }
    free(book);
    return 0;
}
