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
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    long long priority;
    char *title;
    char *note;
    size_t id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *aa = (const Reminder *)a;
    const Reminder *bb = (const Reminder *)b;
    if (aa->priority < bb->priority) return 1;
    if (aa->priority > bb->priority) return -1;
    if (aa->id < bb->id) return -1;
    return 1;
}

int main(void) {
    Reminder *reminders = NULL;
    size_t count = 0;
    size_t cap = 0;
    size_t id_gen = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *prio_str = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *title = p;
            while (*p && *p != ' ') p++;
            char *note = "";
            if (*p == ' ') {
                *p = '\0';
                note = p + 1;
            }
            long long prio = strtoll(prio_str, NULL, 10);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Reminder *nb = realloc(reminders, cap * sizeof(Reminder));
                if (!nb) {
                    free(line);
                    break;
                }
                reminders = nb;
            }
            reminders[count].priority = prio;
            reminders[count].title = safe_dup(title);
            reminders[count].note = safe_dup(note);
            reminders[count].id = id_gen++;
            count++;
        } else if (strncmp(line, "BUMP ", 5) == 0) {
            char *p = line + 5;
            char *title = p;
            while (*p && *p != ' ') p++;
            char *delta_str = "";
            if (*p == ' ') {
                *p = '\0';
                delta_str = p + 1;
            }
            long long delta = strtoll(delta_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    reminders[i].priority += delta;
                    break;
                }
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *title = line + 7;
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
        } else if (strncmp(line, "TOP ", 4) == 0) {
            char *n_str = line + 4;
            long long n = strtoll(n_str, NULL, 10);
            if (n > 0) {
                Reminder *copy = malloc(count * sizeof(Reminder));
                if (copy) {
                    memcpy(copy, reminders, count * sizeof(Reminder));
                    qsort(copy, count, sizeof(Reminder), compare_reminders);
                    size_t limit = (size_t)n < count ? (size_t)n : count;
                    for (size_t i = 0; i < limit; i++) {
                        printf("%lld %s %s\n", copy[i].priority, copy[i].title, copy[i].note);
                    }
                    free(copy);
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
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
