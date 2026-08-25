// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    int priority;
    char *title;
    char *note;
    int id;
} Reminder;

int main(void) {
    Reminder *reminders = NULL;
    int count = 0;
    int cap = 0;
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
            long prio = strtol(p, &endptr, 10);
            p = endptr;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *note = p + 1;
                while (*note == ' ') note++;
                if (count >= cap) {
                    cap = cap == 0 ? 16 : cap * 2;
                    reminders = realloc(reminders, cap * sizeof(Reminder));
                }
                reminders[count].priority = (int)prio;
                reminders[count].title = strdup(title);
                reminders[count].note = strdup(note);
                reminders[count].id = next_id++;
                count++;
            }
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                long delta = strtol(p + 1, NULL, 10);
                for (int i = 0; i < count; i++) {
                    if (strcmp(reminders[i].title, title) == 0) {
                        reminders[i].priority += (int)delta;
                        break;
                    }
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                free(reminders[idx].title);
                free(reminders[idx].note);
                for (int i = idx; i < count - 1; i++) {
                    reminders[i] = reminders[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "TOP ", 4) == 0) {
            long n = strtol(line + 4, NULL, 10);
            Reminder *copy = malloc(count * sizeof(Reminder));
            for (int i = 0; i < count; i++) copy[i] = reminders[i];
            for (int i = 0; i < count - 1; i++) {
                for (int j = i + 1; j < count; j++) {
                    int swap = 0;
                    if (copy[i].priority < copy[j].priority) {
                        swap = 1;
                    } else if (copy[i].priority == copy[j].priority) {
                        if (copy[i].id > copy[j].id) {
                            swap = 1;
                        }
                    }
                    if (swap) {
                        Reminder tmp = copy[i];
                        copy[i] = copy[j];
                        copy[j] = tmp;
                    }
                }
            }
            int limit = n < count ? (int)n : count;
            for (int i = 0; i < limit; i++) {
                printf("%d %s %s\n", copy[i].priority, copy[i].title, copy[i].note);
            }
            free(copy);
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    return 0;
}
