/* F027.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    int id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    Reminder *rA = (Reminder *)a;
    Reminder *rB = (Reminder *)b;
    if (rB->priority != rA->priority) {
        return rB->priority - rA->priority;
    }
    return rA->id - rB->id;
}

int main(void) {
    Reminder *board = NULL;
    int count = 0;
    int capacity = 0;
    int next_id = 0;
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
            if (space1) {
                *space1 = '\0';
                int prio = atoi(args);
                char *rest = space1 + 1;
                char *space2 = strchr(rest, ' ');
                char *ttl = NULL;
                char *nt = NULL;
                if (space2) {
                    *space2 = '\0';
                    ttl = rest;
                    nt = space2 + 1;
                } else {
                    ttl = rest;
                    nt = "";
                }
                char *n_ttl = strdup(ttl);
                char *n_nt = strdup(nt);
                if (n_ttl && n_nt) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        Reminder *tmp = realloc(board, new_cap * sizeof(Reminder));
                        if (tmp) {
                            board = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_ttl); free(n_nt);
                            *space1 = ' '; if (space2) *space2 = ' ';
                            continue;
                        }
                    }
                    board[count].priority = prio;
                    board[count].title = n_ttl;
                    board[count].note = n_nt;
                    board[count].id = next_id++;
                    count++;
                } else {
                    free(n_ttl); free(n_nt);
                }
                *space1 = ' ';
                if (space2) *space2 = ' ';
            }
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *args = line + 5;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *ttl = args;
                int delta = atoi(space + 1);
                for (int i = 0; i < count; i++) {
                    if (strcmp(board[i].title, ttl) == 0) {
                        board[i].priority += delta;
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *ttl = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(board[i].title, ttl) == 0) {
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
            int n = atoi(line + 4);
            if (count > 0) {
                Reminder *cpy = malloc(count * sizeof(Reminder));
                if (cpy) {
                    memcpy(cpy, board, count * sizeof(Reminder));
                    qsort(cpy, count, sizeof(Reminder), compare_reminders);
                    int limit = n < count ? n : count;
                    for (int i = 0; i < limit; i++) {
                        printf("%d %s %s\n", cpy[i].priority, cpy[i].title, cpy[i].note);
                    }
                    free(cpy);
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", board[i].priority, board[i].title, board[i].note);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(board[i].title);
        free(board[i].note);
    }
    free(board);
    return 0;
}
