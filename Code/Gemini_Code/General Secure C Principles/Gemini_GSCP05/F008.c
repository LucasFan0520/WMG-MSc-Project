// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    int score;
    char *note;
} Student;

int main(void) {
    Student *gb = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *endptr;
                long s_val = strtol(p, &endptr, 10);
                if (endptr != p) {
                    p = endptr;
                    while (*p == ' ') p++;
                    char *note_start = p;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Student *new_gb = realloc(gb, capacity * sizeof(Student));
                        if (new_gb) gb = new_gb;
                    }
                    if (count < capacity) {
                        gb[count].name = strdup(name_start);
                        gb[count].score = (int)s_val;
                        gb[count].note = strdup(note_start);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "UPDATE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *endptr;
                long s_val = strtol(p, &endptr, 10);
                if (endptr != p) {
                    p = endptr;
                    while (*p == ' ') p++;
                    char *note_start = p;
                    for (int i = 0; i < count; i++) {
                        if (strcmp(gb[i].name, name_start) == 0) {
                            gb[i].score = (int)s_val;
                            free(gb[i].note);
                            gb[i].note = strdup(note_start);
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "DROP ", 5) == 0) {
            char *name_start = line + 5;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(gb[i].name, name_start) == 0) {
                    free(gb[i].name);
                    free(gb[i].note);
                    for (int j = i; j < count - 1; j++) {
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
                for (int i = 0; i < count; i++) {
                    sum += gb[i].score;
                }
                printf("%lld\n", sum / count);
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %d %s\n", gb[i].name, gb[i].score, gb[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(gb[i].name);
        free(gb[i].note);
    }
    free(gb);
    return 0;
}
