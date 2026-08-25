// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len - 1] == '\r') {
        buf[len - 1] = '\0';
    }
    return buf;
}

typedef struct {
    long long priority;
    char *title;
    char *note;
    size_t insertion_id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *rA = *(const Reminder **)a;
    const Reminder *rB = *(const Reminder **)b;
    if (rA->priority != rB->priority) {
        return (rB->priority > rA->priority) - (rB->priority < rA->priority);
    }
    return (rA->insertion_id > rB->insertion_id) - (rA->insertion_id < rB->insertion_id);
}

int main(void) {
    Reminder *reminders = NULL;
    size_t count = 0;
    size_t capacity = 0;
    size_t next_id = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *prio_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *note = p;
            long long prio = atoll(prio_s);
            if (*title != '\0') {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Reminder *new_reminders = realloc(reminders, capacity * sizeof(Reminder));
                    if (new_reminders) reminders = new_reminders;
                }
                if (count < capacity) {
                    reminders[count].priority = prio;
                    reminders[count].title = strdup(title);
                    reminders[count].note = strdup(note);
                    reminders[count].insertion_id = next_id++;
                    count++;
                }
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *delta_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long delta = atoll(delta_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    reminders[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *title = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    free(reminders[i].title);
                    free(reminders[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        reminders[j] = reminders[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *n_s = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            long long n = atoll(n_s);
            if (n > 0 && count > 0) {
                Reminder **arr = malloc(count * sizeof(Reminder *));
                if (arr) {
                    for (size_t i = 0; i < count; i++) arr[i] = &reminders[i];
                    qsort(arr, count, sizeof(Reminder *), compare_reminders);
                    size_t limit = (size_t)n < count ? (size_t)n : count;
                    for (size_t i = 0; i < limit; i++) {
                        printf("(%lld) %s: %s\n", arr[i]->priority, arr[i]->title, arr[i]->note);
                    }
                    free(arr);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("(%lld) %s: %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
    return 0;
}
