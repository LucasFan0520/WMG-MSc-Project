/* F027.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int priority;
    char *title;
    char *note;
    size_t id;
} Reminder;

int compare_reminders(const void *a, const void *b) {
    Reminder *rA = *(Reminder **)a;
    Reminder *rB = *(Reminder **)b;
    if (rA->priority > rB->priority) return -1;
    if (rA->priority < rB->priority) return 1;
    if (rA->id < rB->id) return -1;
    return 1;
}

int main(void) {
    Reminder *rems = NULL;
    size_t count = 0;
    size_t capacity = 0;
    size_t total_id = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) { *p = '\0'; p++; }
        if (strcmp(cmd, "END") == 0) break;
        while (*p == ' ') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *prio_str = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            char *note = p;

            int prio = atoi(prio_str);
            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                Reminder *temp = realloc(rems, new_cap * sizeof(Reminder));
                if (!temp) break;
                rems = temp;
                capacity = new_cap;
            }
            rems[count].priority = prio;
            rems[count].title = strdup(title);
            rems[count].note = strdup(note);
            rems[count].id = total_id++;
            if (rems[count].title && rems[count].note) count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; }
            while (*p == ' ') p++;
            int delta = atoi(p);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(rems[i].title, title) == 0) {
                    rems[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = p;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(rems[i].title, title) == 0) {
                    free(rems[i].title);
                    free(rems[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        rems[j] = rems[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n = atoi(p);
            if (n > 0 && count > 0) {
                size_t limit = (size_t)n > count ? count : (size_t)n;
                Reminder **temp_arr = malloc(count * sizeof(Reminder *));
                if (temp_arr) {
                    for (size_t i = 0; i < count; i++) {
                        temp_arr[i] = &rems[i];
                    }
                    qsort(temp_arr, count, sizeof(Reminder *), compare_reminders);
                    for (size_t i = 0; i < limit; i++) {
                        printf("%d %s %s\n", temp_arr[i]->priority, temp_arr[i]->title, temp_arr[i]->note);
                    }
                    free(temp_arr);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%d %s %s\n", rems[i].priority, rems[i].title, rems[i].note);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(rems[i].title);
        free(rems[i].note);
    }
    free(rems);
    return 0;
}
