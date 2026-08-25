// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    int index;
} Reminder;

Reminder *board = NULL;
int count = 0;
int capacity = 0;
int total_inserted = 0;

int find_reminder(const char *title) {
    for (int i = 0; i < count; i++) {
        if (strcmp(board[i].title, title) == 0) {
            return i;
        }
    }
    return -1;
}

int compare_reminder(const void *a, const void *b) {
    Reminder *rA = (Reminder *)a;
    Reminder *rB = (Reminder *)b;
    if (rB->priority != rA->priority) {
        return rB->priority - rA->priority;
    }
    return rA->index - rB->index;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *sp1 = strchr(p1, ' ');
            if (sp1) {
                *sp1 = 0;
                char *p2 = sp1 + 1;
                char *sp2 = strchr(p2, ' ');
                if (sp2) {
                    *sp2 = 0;
                    char *note = sp2 + 1;
                    int prio = atoi(p1);
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        board = realloc(board, capacity * sizeof(Reminder));
                    }
                    board[count].priority = prio;
                    board[count].title = strdup(p2);
                    board[count].note = strdup(note);
                    board[count].index = total_inserted++;
                    count++;
                }
            }
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *p1 = line + 5;
            char *sp = strchr(p1, ' ');
            if (sp) {
                *sp = 0;
                int delta = atoi(sp + 1);
                int idx = find_reminder(p1);
                if (idx != -1) {
                    board[idx].priority += delta;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            int idx = find_reminder(title);
            if (idx != -1) {
                free(board[idx].title);
                free(board[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    board[i] = board[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            int n = atoi(line + 4);
            Reminder *temp = malloc(count * sizeof(Reminder));
            memcpy(temp, board, count * sizeof(Reminder));
            qsort(temp, count, sizeof(Reminder), compare_reminder);
            int limit = n < count ? n : count;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
            }
            free(temp);
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", board[i].priority, board[i].title, board[i].note);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(board[i].title);
        free(board[i].note);
    }
    free(board);
    return 0;
}
