// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Emp {
    char *id;
    char *name;
    char *dept;
    char *title;
    struct Emp *next;
} Emp;

Emp *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Emp *find_emp(const char *id) {
    for (Emp *e = head; e; e = e->next)
        if (strcmp(e->id, id) == 0) return e;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *id = get_token(&ptr);
            char *name = get_token(&ptr);
            char *dept = get_token(&ptr);
            if (id && name && dept && !find_emp(id)) {
                Emp *e = malloc(sizeof(Emp));
                if (e) {
                    e->id = strdup(id);
                    e->name = strdup(name);
                    e->dept = strdup(dept);
                    e->title = strdup(ptr);
                    e->next = head;
                    head = e;
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *id = get_token(&ptr);
            char *name = get_token(&ptr);
            char *dept = get_token(&ptr);
            if (id && name && dept) {
                Emp *e = find_emp(id);
                if (e) {
                    char *t_name = strdup(name);
                    char *t_dept = strdup(dept);
                    char *t_title = strdup(ptr);
                    if (t_name && t_dept && t_title) {
                        free(e->name); e->name = t_name;
                        free(e->dept); e->dept = t_dept;
                        free(e->title); e->title = t_title;
                    } else {
                        free(t_name); free(t_dept); free(t_title);
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = get_token(&ptr);
            if (id) {
                Emp *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->id, id) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free(curr->id);
                        free(curr->name);
                        free(curr->dept);
                        free(curr->title);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = get_token(&ptr);
            if (id) {
                Emp *e = find_emp(id);
                if (e) printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Emp *e = head; e; e = e->next) {
                printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
            }
        }
    }
    while (head) {
        Emp *next = head->next;
        free(head->id);
        free(head->name);
        free(head->dept);
        free(head->title);
        free(head);
        head = next;
    }
    return 0;
}