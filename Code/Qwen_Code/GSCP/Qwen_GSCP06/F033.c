// F033.c
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

typedef struct Emp {
    char* id;
    char* name;
    char* dept;
    char* title;
    struct Emp* next;
} Emp;

Emp* head = NULL;

Emp* find_emp(const char* id) {
    for (Emp* e = head; e; e = e->next)
        if (strcmp(e->id, id) == 0) return e;
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

        if (strcmp(cmd, "ADD") == 0) {
            char* id = get_word(&p);
            char* name = get_word(&p);
            char* dept = get_word(&p);
            if (id && name && dept && !find_emp(id)) {
                Emp* n = malloc(sizeof(Emp));
                n->id = strdup(id);
                n->name = strdup(name);
                n->dept = strdup(dept);
                n->title = strdup(p);
                n->next = head;
                head = n;
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char* id = get_word(&p);
            char* name = get_word(&p);
            char* dept = get_word(&p);
            Emp* e = find_emp(id);
            if (e && name && dept) {
                free(e->name);
                free(e->dept);
                free(e->title);
                e->name = strdup(name);
                e->dept = strdup(dept);
                e->title = strdup(p);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* id = get_word(&p);
            Emp* prev = NULL;
            for (Emp* e = head; e; prev = e, e = e->next) {
                if (strcmp(e->id, id) == 0) {
                    if (prev) prev->next = e->next;
                    else head = e->next;
                    free(e->id);
                    free(e->name);
                    free(e->dept);
                    free(e->title);
                    free(e);
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char* id = get_word(&p);
            Emp* e = find_emp(id);
            if (e) printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Emp* e = head; e; e = e->next) {
                printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
            }
        }
    }
    return 0;
}