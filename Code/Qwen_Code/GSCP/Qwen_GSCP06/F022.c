// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

typedef struct Task {
    char* title;
    char* note;
    int done;
    struct Task* next;
} Task;

typedef struct Project {
    char* name;
    Task* tasks;
    struct Project* next;
} Project;

Project* head = NULL;

Project* find_proj(const char* name) {
    for (Project* p = head; p; p = p->next)
        if (strcmp(p->name, name) == 0) return p;
    return NULL;
}

Task* find_task(Project* p, const char* title) {
    for (Task* t = p->tasks; t; t = t->next)
        if (strcmp(t->title, title) == 0) return t;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "PROJECT") == 0) {
            char* name = get_word(&p);
            if (name && !find_proj(name)) {
                Project* n = malloc(sizeof(Project));
                n->name = strdup(name);
                n->tasks = NULL;
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char* pname = get_word(&p);
            char* title = get_word(&p);
            Project* pr = find_proj(pname);
            if (pr && title) {
                Task* n = malloc(sizeof(Task));
                n->title = strdup(title);
                n->note = strdup(p);
                n->done = 0;
                n->next = pr->tasks;
                pr->tasks = n;
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char* from = get_word(&p);
            char* to = get_word(&p);
            char* title = get_word(&p);
            Project* pf = find_proj(from);
            Project* pt = find_proj(to);
            if (pf && pt && title) {
                Task* prev = NULL;
                for (Task* t = pf->tasks; t; prev = t, t = t->next) {
                    if (strcmp(t->title, title) == 0) {
                        if (prev) prev->next = t->next;
                        else pf->tasks = t->next;
                        t->next = pt->tasks;
                        pt->tasks = t;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char* pname = get_word(&p);
            char* title = get_word(&p);
            Project* pr = find_proj(pname);
            if (pr && title) {
                Task* t = find_task(pr, title);
                if (t) t->done = 1;
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char* name = get_word(&p);
            Project* prev = NULL;
            for (Project* pr = head; pr; prev = pr, pr = pr->next) {
                if (strcmp(pr->name, name) == 0) {
                    if (prev) prev->next = pr->next;
                    else head = pr->next;
                    free(pr->name);
                    while (pr->tasks) {
                        Task* temp = pr->tasks;
                        pr->tasks = pr->tasks->next;
                        free(temp->title);
                        free(temp->note);
                        free(temp);
                    }
                    free(pr);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Project* pr = head; pr; pr = pr->next) {
                printf("%s\n", pr->name);
                for (Task* t = pr->tasks; t; t = t->next) {
                    printf("%s %s %s\n", t->title, t->done ? "DONE" : "TODO", t->note);
                }
            }
        }
    }
    return 0;
}