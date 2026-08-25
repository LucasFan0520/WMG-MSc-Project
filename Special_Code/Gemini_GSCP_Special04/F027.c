/* F027.c */
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
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    long long priority;
    char *title;
    char *note;
    size_t ins_id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *ra = (const Reminder *)a;
    const Reminder *rb = (const Reminder *)b;
    if (ra->priority > rb->priority) return -1;
    if (ra->priority < rb->priority) return 1;
    return (ra->ins_id < rb->ins_id) ? -1 : 1;
}

int main(void) {
    Reminder *reminders = NULL;
    size_t count = 0;
    size_t cap = 0;
    size_t seq = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "ADD") == 0) {
            while (*p == ' ') p++;
            char *prio_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            long long prio = strtoll(prio_str, NULL, 10);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Reminder *nr = realloc(reminders, cap * sizeof(Reminder));
                if (nr) reminders = nr;
            }
            char *ntit = mystrdup(title);
            char *nnote = mystrdup(note);
            if (ntit && nnote) {
                reminders[count].priority = prio;
                reminders[count].title = nitit;
                reminders[count].note = nnote;
                reminders[count].ins_id = seq++;
                count++;
            } else {
                free(ntit); free(nnote);
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *delta_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long delta = strtoll(delta_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    reminders[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(reminders[found].title);
                free(reminders[found].note);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    reminders[i] = reminders[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            while (*p == ' ') p++;
            char *n_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            long long n = strtoll(n_str, NULL, 10);
            if (n > 0 && count > 0) {
                Reminder *copy = malloc(count * sizeof(Reminder));
                if (copy) {
                    memcpy(copy, reminders, count * sizeof(Reminder));
                    qsort(copy, count, sizeof(Reminder), compare_reminders);
                    size_t limit = (size_t)n > count ? count : (size_t)n;
                    for (size_t i = 0; i < limit; i++) {
                        printf("%lld %s %s\n", copy[i].priority, copy[i].title, copy[i].note);
                    }
                    free(copy);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%lld %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
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
