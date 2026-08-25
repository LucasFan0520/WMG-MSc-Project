// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    Student *arr = NULL;
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
            char *name_start = line + 4;
            char *s1 = strchr(name_start, ' ');
            if (s1) {
                *s1 = '\0';
                char *score_str = s1 + 1;
                char *s2 = strchr(score_str, ' ');
                char *note_start = "";
                if (s2) {
                    *s2 = '\0';
                    note_start = s2 + 1;
                }
                int score = atoi(score_str);
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    arr = realloc(arr, cap * sizeof(Student));
                }
                arr[count].name = strdup(name_start);
                arr[count].score = score;
                arr[count].note = strdup(note_start);
                count++;
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *name_start = line + 7;
            char *s1 = strchr(name_start, ' ');
            if (s1) {
                *s1 = '\0';
                char *score_str = s1 + 1;
                char *s2 = strchr(score_str, ' ');
                char *note_start = "";
                if (s2) {
                    *s2 = '\0';
                    note_start = s2 + 1;
                }
                int score = atoi(score_str);
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(arr[i].name, name_start) == 0) {
                        arr[i].score = score;
                        free(arr[i].note);
                        arr[i].note = strdup(note_start);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name_start = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].name, name_start) == 0) {
                    free(arr[i].name);
                    free(arr[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
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
                    sum += arr[i].score;
                }
                printf("%lld\n", sum / count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d %s\n", arr[i].name, arr[i].score, arr[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].name);
        free(arr[i].note);
    }
    free(arr);
    return 0;
}
