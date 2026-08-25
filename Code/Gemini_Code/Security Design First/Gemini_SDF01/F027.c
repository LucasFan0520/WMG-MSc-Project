// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    long priority;
    char *title;
    char *note;
    size_t id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    const Reminder *rA = (const Reminder *)a;
    const Reminder *rB = (const Reminder *)b;
    if (rA->priority < rB->priority) return 1;
    if (rA->priority > rB->priority) return -1;
    if (rA->id < rB->id) return -1;
    return 1;
}

int main(void) {
    Reminder *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    size_t next_id = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *prio_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *note = p;

            long prio = strtol(prio_str, NULL, 10);

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                Reminder *new_items = realloc(items, capacity * sizeof(Reminder));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].priority = prio;
            items[count].title = strdup(title);
            items[count].note = strdup(note);
            items[count].id = next_id++;
            if (!items[count].title || !items[count].note) exit(1);
            count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *delta_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long delta = strtol(delta_str, NULL, 10);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
                    items[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].title, title) == 0) {
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
            char *n_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long n = strtol(n_str, NULL, 10);
            if (n < 0) n = 0;

            if (count > 0) {
                Reminder *dup_items = malloc(count * sizeof(Reminder));
                if (!dup_items) exit(1);
                memcpy(dup_items, items, count * sizeof(Reminder));
                qsort(dup_items, count, sizeof(Reminder), compare_reminders);

                size_t limit = (size_t)n > count ? count : (size_t)n;
                for (size_t i = 0; i < limit; i++) {
                    printf("%ld %s %s\n", dup_items[i].priority, dup_items[i].title, dup_items[i].note);
                }
                free(dup_items);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%ld %s %s\n", items[i].priority, items[i].title, items[i].note);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].title);
        free(items[i].note);
    }
    free(items);
    free(line);
    return 0;
}
