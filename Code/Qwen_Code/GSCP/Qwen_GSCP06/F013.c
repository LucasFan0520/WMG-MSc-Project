// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Task {
    char* title;
    char* note;
    int status;
    struct Task* next;
} Task;

Task* head = NULL;

Task* find_task(const char* title) {
    for (Task* t = head; t; t = t->next) {
        if (strcmp(t->title, title) == 0) return t;
    }
    return NULL;
}

const char* status_name(int s) {
    if (s == 0) return "todo";
    if (s == 1) return "doing";
    return "done";
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "TODO") == 0) {
            char* title = get_word(&p);
            Task* n = malloc(sizeof(Task));
            n->title = strdup(title);
            n->note = strdup(p);
            n->status = 0;
            n->next = head;
            head = n;
        } else if (strcmp(cmd, "START") == 0) {
            char* title = get_word(&p);
            Task* t = find_task(title);
            if (t) t->status = 1;
        } else if (strcmp(cmd, "DONE") == 0) {
            char* title = get_word(&p);
            Task* t = find_task(title);
            if (t) t->status = 2;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char* title = get_word(&p);
            Task* prev = NULL;
            for (Task* t = head; t; prev = t, t = t->next) {
                if (strcmp(t->title, title) == 0) {
                    if (prev) prev->next = t->next;
                    else head = t->next;
                    free(t->title);
                    free(t->note);
                    free(t);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* title = get_word(&p);
            Task* t = find_task(title);
            if (t) printf("%s %s\n", status_name(t->status), t->note);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int s = 0; s < 3; s++) {
                for (Task* t = head; t; t = t->next) {
                    if (t->status == s) {
                        printf("%s %s %s\n", status_name(s), t->title, t->note);
                    }
                }
            }
        }
    }
    return 0;
}