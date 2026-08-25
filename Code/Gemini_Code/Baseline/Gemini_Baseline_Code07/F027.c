// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    int priority;
    char *title;
    char *note;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    Reminder *rA = (Reminder *)a;
    Reminder *rB = (Reminder *)b;
    return rB->priority - rA->priority;
}

int main() {
    Reminder *board = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", board[i].priority, board[i].title, board[i].note);
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            int prio = 0;
            char title[256];
            int n2 = 0;
            if (sscanf(p, "%d %255s%n", &prio, title, &n2) == 2) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    board = realloc(board, capacity * sizeof(Reminder));
                }
                board[count].priority = prio;
                board[count].title = strdup(title);
                board[count].note = strdup(p);
                count++;
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256];
            int delta = 0;
            if (sscanf(p, "%255s %d", title, &delta) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(board[i].title, title) == 0) {
                        board[i].priority += delta;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(p, "%255s", title) == 1) {
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
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int top_n = 0;
            if (sscanf(p, "%d", &top_n) == 1) {
                if (top_n > count) top_n = count;
                Reminder *temp = malloc(count * sizeof(Reminder));
                memcpy(temp, board, count * sizeof(Reminder));
                qsort(temp, count, sizeof(Reminder), compare_reminders);
                for (int i = 0; i < top_n; i++) {
                    printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
                }
                free(temp);
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
