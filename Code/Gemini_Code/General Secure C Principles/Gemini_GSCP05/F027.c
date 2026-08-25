// F027.c
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
    long long priority;
    char *title;
    char *note;
    int id;
} Reminder;

int compare_reminder(const void *a, const void *b) {
    const Reminder *rA = (const Reminder *)a;
    const Reminder *rB = (const Reminder *)b;
    if (rA->priority != rB->priority) {
        return (rB->priority > rA->priority) ? 1 : -1;
    }
    return rA->id - rB->id;
}

int main(void) {
    Reminder *board = NULL;
    int count = 0;
    int capacity = 0;
    int next_id = 0;
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
            char *endptr;
            long long prio = strtoll(p, &endptr, 10);
            if (endptr != p) {
                p = endptr;
                while (*p == ' ') p++;
                char *title = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *note = p + 1;
                    while (*note == ' ') note++;
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Reminder *new_b = realloc(board, capacity * sizeof(Reminder));
                        if (new_b) board = new_b;
                    }
                    if (count < capacity) {
                        board[count].priority = prio;
                        board[count].title = strdup(title);
                        board[count].note = strdup(note);
                        board[count].id = next_id++;
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *delta_str = p + 1;
                while (*delta_str == ' ') delta_str++;
                long long delta = strtoll(delta_str, NULL, 10);
                for (int i = 0; i < count; i++) {
                    if (strcmp(board[i].title, title) == 0) {
                        board[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(board[i].title, title) == 0) {
                    free(board[i].title);
                    free(board[i].note);
                    for (int j = i; j < count - 1; j++) {
                        board[j] = board[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            char *n_str = line + 4;
            while (*n_str == ' ') n_str++;
            long long n = strtoll(n_str, NULL, 10);
            if (n > count) n = count;
            if (count > 0) {
                Reminder *temp = malloc(count * sizeof(Reminder));
                if (temp) {
                    memcpy(temp, board, count * sizeof(Reminder));
                    qsort(temp, count, sizeof(Reminder), compare_reminder);
                    for (int i = 0; i < n; i++) {
                        printf("%lld %s: %s\n", temp[i].priority, temp[i].title, temp[i].note);
                    }
                    free(temp);
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%lld %s: %s\n", board[i].priority, board[i].title, board[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(board[i].title);
        free(board[i].note);
    }
    free(board);
    return 0;
}
