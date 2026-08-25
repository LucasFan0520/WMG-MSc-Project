// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Reminder {
    int priority;
    char *title;
    char *note;
    int order;
    struct Reminder *next;
} Reminder;

Reminder *reminders = NULL;
int global_order = 0;

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            int prio;
            char title[256];
            int n = 0;
            if (sscanf(rest, "%d %255s%n", &prio, title, &n) != 2) continue;
            char *note = rest + n;
            while (*note == ' ') note++;
            Reminder *r = malloc(sizeof(Reminder));
            if (!r) continue;
            r->priority = prio;
            r->title = strdup(title);
            r->note = strdup(note);
            if (!r->title || !r->note) { free(r->title); free(r->note); free(r); continue; }
            r->order = global_order++;
            r->next = reminders;
            reminders = r;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char title[256];
            int delta;
            if (sscanf(rest, "%255s %d", title, &delta) != 2) continue;
            for (Reminder *r = reminders; r; r = r->next) {
                if (strcmp(r->title, title) == 0) {
                    r->priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char title[256];
            if (sscanf(rest, "%255s", title) != 1) continue;
            Reminder *prev = NULL;
            for (Reminder *r = reminders; r; prev = r, r = r->next) {
                if (strcmp(r->title, title) == 0) {
                    if (prev) prev->next = r->next;
                    else reminders = r->next;
                    free(r->title);
                    free(r->note);
                    free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            int n;
            if (sscanf(rest, "%d", &n) != 1 || n <= 0) continue;
            int count = 0;
            for (Reminder *r = reminders; r; r = r->next) count++;
            if (count == 0) continue;
            
            Reminder **arr = malloc(count * sizeof(Reminder*));
            if (!arr) continue;
            int idx = 0;
            for (Reminder *r = reminders; r; r = r->next) arr[idx++] = r;
            
            for (int i = 0; i < count - 1; i++) {
                for (int j = i + 1; j < count; j++) {
                    if (arr[j]->priority > arr[i]->priority || 
                       (arr[j]->priority == arr[i]->priority && arr[j]->order < arr[i]->order)) {
                        Reminder *tmp = arr[i];
                        arr[i] = arr[j];
                        arr[j] = tmp;
                    }
                }
            }
            
            int limit = n < count ? n : count;
            for (int i = 0; i < limit; i++) {
                printf("%d %s ", arr[i]->priority, arr[i]->title);
                print_text(arr[i]->note);
            }
            free(arr);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Reminder *r = reminders; r; r = r->next) {
                printf("%d %s ", r->priority, r->title);
                print_text(r->note);
            }
        }
    }
    while (reminders) {
        Reminder *r = reminders;
        reminders = reminders->next;
        free(r->title);
        free(r->note);
        free(r);
    }
    return 0;
}