// F027.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long priority;
    char *title;
    char *note;
    int id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *pa = (const Reminder *)a;
    const Reminder *pb = (const Reminder *)b;
    if (pa->priority > pb->priority) return -1;
    if (pa->priority < pb->priority) return 1;
    return (pa->id < pb->id) ? -1 : 1;
}

int main(void) {
    Reminder *board = NULL;
    int count = 0;
    int next_id = 0;
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
                char *p_str = p;
                char *p2 = s1 + 1;
                char *s2 = strchr(p2, ' ');
                if (s2) {
                    *s2 = '\0';
                    char *title = p2;
                    char *note = s2 + 1;
                    long pri = strtol(p_str, NULL, 10);
                    Reminder *tmp = realloc(board, sizeof(Reminder) * (count + 1));
                    if (tmp) {
                        board = tmp;
                        board[count].priority = pri;
                        board[count].title = strdup(title);
                        board[count].note = strdup(note);
                        board[count].id = next_id++;
                        if (board[count].title && board[count].note) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *p = line + 5;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *title = p;
                char *d_str = space + 1;
                long delta = strtol(d_str, NULL, 10);
                for (int i = 0; i < count; i++) {
                    if (strcmp(board[i].title, title) == 0) {
                        board[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(board[i].title, title) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(board[idx].title);
                free(board[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    board[i] = board[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            char *n_str = line + 4;
            long n = strtol(n_str, NULL, 10);
            if (n > 0) {
                Reminder *cpy = malloc(sizeof(Reminder) * count);
                if (cpy) {
                    memcpy(cpy, board, sizeof(Reminder) * count);
                    qsort(cpy, count, sizeof(Reminder), compare_reminders);
                    long limit = (n < count) ? n : count;
                    for (long i = 0; i < limit; i++) {
                        printf("(%ld) %s: %s\n", cpy[i].priority, cpy[i].title, cpy[i].note);
                    }
                    free(cpy);
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("(%ld) %s: %s\n", board[i].priority, board[i].title, board[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(board[i].title);
        free(board[i].note);
    }
    free(board);
    free(line);
    return 0;
}
