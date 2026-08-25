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

int main() {
    Reminder *board = NULL;
    int count = 0;
    int next_id = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        int len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
            len--;
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[32];
        int read_bytes = 0;
        if (sscanf(line, "%31s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *ptr = line + read_bytes;
        while (*ptr == ' ') ptr++;
        if (strcmp(cmd, "ADD") == 0) {
            int priority = 0;
            char title[256];
            int p_len = 0, t_len = 0;
            if (sscanf(ptr, "%d%n", &priority, &p_len) > 0) {
                char *ptr2 = ptr + p_len;
                while (*ptr2 == ' ') ptr2++;
                if (sscanf(ptr2, "%255s%n", title, &t_len) > 0) {
                    char *note = ptr2 + t_len;
                    while (*note == ' ') note++;
                    board = realloc(board, (count + 1) * sizeof(Reminder));
                    board[count].priority = priority;
                    board[count].title = strdup(title);
                    board[count].note = strdup(note);
                    board[count].id = next_id++;
                    count++;
                }
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256];
            int delta = 0;
            if (sscanf(ptr, "%255s %d", title, &delta) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(board[i].title, title) == 0) {
                        board[i].priority += delta;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(ptr, "%255s", title) > 0) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(board[i].title, title) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(board[found].title);
                    free(board[found].note);
                    for (int i = found; i < count - 1; i++) {
                        board[i] = board[i + 1];
                    }
                    count--;
                    if (count > 0) {
                        board = realloc(board, count * sizeof(Reminder));
                    } else {
                        free(board);
                        board = NULL;
                    }
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = 0;
            if (sscanf(ptr, "%d", &n) > 0) {
                Reminder *temp = malloc(count * sizeof(Reminder));
                for (int i = 0; i < count; i++) {
                    temp[i] = board[i];
                }
                for (int i = 0; i < count - 1; i++) {
                    for (int j = 0; j < count - i - 1; j++) {
                        if (temp[j].priority < temp[j + 1].priority || 
                           (temp[j].priority == temp[j + 1].priority && temp[j].id > temp[j + 1].id)) {
                            Reminder t = temp[j];
                            temp[j] = temp[j + 1];
                            temp[j + 1] = t;
                        }
                    }
                }
                int limit = (n < count) ? n : count;
                for (int i = 0; i < limit; i++) {
                    printf("(%d) %s: %s\n", temp[i].priority, temp[i].title, temp[i].note);
                }
                free(temp);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("(%d) %s: %s\n", board[i].priority, board[i].title, board[i].note);
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
