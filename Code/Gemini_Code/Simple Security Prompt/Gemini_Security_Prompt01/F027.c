// F027.c
#define _GNU_SOURCE
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
    const Reminder *rA = (const Reminder *)a;
    const Reminder *rB = (const Reminder *)b;
    if (rA->priority > rB->priority) return -1;
    if (rA->priority < rB->priority) return 1;
    return (rA->id < rB->id) ? -1 : 1;
}

int main() {
    Reminder *board = NULL;
    int count = 0;
    int next_id = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "ADD") == 0 && args) {
            char *prio_str = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *title = space2 + 1;
                while (*title == ' ') title++;
                char *space3 = strchr(title, ' ');
                char *note = "";
                if (space3) {
                    *space3 = '\0';
                    note = space3 + 1;
                    while (*note == ' ') note++;
                }
                board = realloc(board, (count + 1) * sizeof(Reminder));
                if (!board) return 1;
                board[count].priority = atoi(prio_str);
                board[count].title = strdup(title);
                board[count].note = strdup(note);
                board[count].id = next_id++;
                count++;
            }
        } else if (strcmp(cmd, "BUMP") == 0 && args) {
            char *title = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                int delta = atoi(space2 + 1);
                for (int i = 0; i < count; i++) {
                    if (strcmp(board[i].title, title) == 0) {
                        board[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(board[i].title, args) == 0) {
                    free(board[i].title);
                    free(board[i].note);
                    for (int j = i; j < count - 1; j++) {
                        board[j] = board[j + 1];
                    }
                    count--;
                    board = realloc(board, count * sizeof(Reminder));
                    if (count > 0 && !board) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0 && args) {
            int n = atoi(args);
            Reminder *temp = malloc(count * sizeof(Reminder));
            if (count > 0 && !temp) return 1;
            for (int i = 0; i < count; i++) {
                temp[i] = board[i];
            }
            if (count > 0) {
                qsort(temp, count, sizeof(Reminder), compare_reminders);
            }
            int limit = (n < count) ? n : count;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
            }
            free(temp);
        } else if (strcmp(cmd, "LIST") == 0) {
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
