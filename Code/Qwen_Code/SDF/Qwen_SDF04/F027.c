// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { int priority; char* title; char* note; } Reminder;

Reminder* reminders = NULL;
size_t r_count = 0, r_cap = 0;

void cleanup() {
    for (size_t i = 0; i < r_count; i++) {
        free(reminders[i].title);
        free(reminders[i].note);
    }
    free(reminders);
}

int cmp_pri(const void* a, const void* b) {
    const Reminder* ra = (const Reminder*)a;
    const Reminder* rb = (const Reminder*)b;
    return rb->priority - ra->priority;
}

int main() {
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* pri_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;

            if (r_count == r_cap) {
                size_t new_cap = r_cap == 0 ? 4 : r_cap * 2;
                Reminder* temp = realloc(reminders, new_cap * sizeof(Reminder));
                if (!temp) { cleanup(); free(line); return 1; }
                reminders = temp; r_cap = new_cap;
            }
            reminders[r_count].priority = atoi(pri_str);
            reminders[r_count].title = strdup(title);
            reminders[r_count].note = strdup(note);
            if (!reminders[r_count].title || !reminders[r_count].note) { cleanup(); free(line); return 1; }
            r_count++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            int delta = atoi(ptr);

            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(reminders[i].title, title) == 0) {
                    reminders[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(reminders[i].title, ptr) == 0) {
                    free(reminders[i].title);
                    free(reminders[i].note);
                    for (size_t j = i; j < r_count - 1; j++) reminders[j] = reminders[j + 1];
                    r_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            int n = atoi(ptr);
            if (n > (int)r_count) n = (int)r_count;
            
            Reminder* temp = malloc(r_count * sizeof(Reminder));
            if (!temp) { cleanup(); free(line); return 1; }
            memcpy(temp, reminders, r_count * sizeof(Reminder));
            qsort(temp, r_count, sizeof(Reminder), cmp_pri);
            
            for (size_t i = 0; i < (size_t)n; i++) {
                printf("%d %s %s\n", temp[i].priority, temp[i].title, temp[i].note);
            }
            free(temp);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                printf("%d %s %s\n", reminders[i].priority, reminders[i].title, reminders[i].note);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}