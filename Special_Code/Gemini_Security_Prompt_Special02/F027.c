// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    int priority;
    char *title;
    char *note;
    size_t insertion_id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    Reminder *rA = (Reminder *)a;
    Reminder *rB = (Reminder *)b;
    if (rB->priority != rA->priority) {
        return rB->priority - rA->priority;
    }
    return (rA->insertion_id < rB->insertion_id) ? -1 : 1;
}

int main() {
    Reminder *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    size_t total_inserted = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *arg1 = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "ADD") == 0) {
            int prio = atoi(arg1);
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *note = p;
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(Reminder));
            }
            items[count].priority = prio;
            items[count].title = mystrdup(title);
            items[count].note = mystrdup(note);
            items[count].insertion_id = total_inserted++;
            count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            while (*p == ' ') p++;
            char *delta_s = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int delta = atoi(delta_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, arg1) == 0) {
                    items[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, arg1) == 0) {
                    free(items[i].title);
                    free(items[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = atoi(arg1);
            if (n < 0) n = 0;
            size_t num_to_show = (size_t)n < count ? (size_t)n : count;
            if (num_to_show > 0) {
                Reminder *temp = malloc(count * sizeof(Reminder));
                memcpy(temp, items, count * sizeof(Reminder));
                qsort(temp, count, sizeof(Reminder), compare_reminders);
                for (size_t i = 0; i < num_to_show; i++) {
                    printf("(%d) %s: %s\n", temp[i].priority, temp[i].title, temp[i].note);
                }
                free(temp);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("(%d) %s: %s\n", items[i].priority, items[i].title, items[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].note);
    }
    free(items);
    return 0;
}
