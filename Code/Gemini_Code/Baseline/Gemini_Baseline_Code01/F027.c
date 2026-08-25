// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    int id;
} Reminder;

int compare_reminder(const void *a, const void *b) {
    Reminder *rA = *(Reminder **)a;
    Reminder *rB = *(Reminder **)b;
    if (rB->priority != rA->priority) {
        return rB->priority - rA->priority;
    }
    return rA->id - rB->id;
}

int main() {
    Reminder *board = NULL;
    int count = 0;
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "ADD") == 0) {
            int prio = 0;
            char title[256];
            int bytes = 0;
            if (sscanf(args, "%d %255s%n", &prio, title, &bytes) == 2) {
                char *note = args + bytes;
                while (*note == ' ') note++;
                board = realloc(board, (count + 1) * sizeof(Reminder));
                board[count].priority = prio;
                board[count].title = strdup(title);
                board[count].note = strdup(note);
                board[count].id = count;
                count++;
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256];
            int delta = 0;
            if (sscanf(args, "%255s %d", title, &delta) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(board[i].title, title) == 0) {
                        board[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(args, "%255s", title) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(board[i].title, title) == 0) {
                        free(board[i].title);
                        free(board[i].note);
                        for (int j = i; j < count - 1; j++) {
                            board[j] = board[j + 1];
                        }
                        count--;
                        board = realloc(board, count * sizeof(Reminder));
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = 0;
            if (sscanf(args, "%d", &n) == 1) {
                if (n > count) n = count;
                if (n > 0) {
                    Reminder **refs = malloc(count * sizeof(Reminder *));
                    for (int i = 0; i < count; i++) {
                        refs[i] = &board[i];
                    }
                    qsort(refs, count, sizeof(Reminder *), compare_reminder);
                    for (int i = 0; i < n; i++) {
                        printf("%d %s %s\n", refs[i]->priority, refs[i]->title, refs[i]->note);
                    }
                    free(refs);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
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
