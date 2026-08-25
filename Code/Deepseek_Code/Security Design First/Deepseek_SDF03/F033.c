// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_ID 64
#define MAX_NAME 64
#define MAX_DEPT 64
#define MAX_TITLE 64

typedef struct Employee {
    char id[MAX_ID];
    char name[MAX_NAME];
    char dept[MAX_DEPT];
    char title[MAX_TITLE];
    struct Employee *next;
} Employee;

Employee *head = NULL;

Employee *find_employee(const char *id) {
    Employee *e = head;
    while (e) {
        if (strcmp(e->id, id) == 0) return e;
        e = e->next;
    }
    return NULL;
}

void free_all() {
    Employee *e = head;
    while (e) {
        Employee *tmp = e;
        e = e->next;
        free(tmp);
    }
    head = NULL;
}

void handle_add(char *line) {
    char id[MAX_ID], name[MAX_NAME], dept[MAX_DEPT], title[MAX_TITLE];
    int n = sscanf(line, "ADD %63s %63s %63s %63s", id, name, dept, title);
    if (n < 4) return;
    if (find_employee(id)) return;
    Employee *e = malloc(sizeof(Employee));
    if (!e) return;
    strcpy(e->id, id);
    strcpy(e->name, name);
    strcpy(e->dept, dept);
    strcpy(e->title, title);
    e->next = head;
    head = e;
}

void handle_update(char *line) {
    char id[MAX_ID], name[MAX_NAME], dept[MAX_DEPT], title[MAX_TITLE];
    int n = sscanf(line, "UPDATE %63s %63s %63s %63s", id, name, dept, title);
    if (n < 4) return;
    Employee *e = find_employee(id);
    if (!e) return;
    strcpy(e->name, name);
    strcpy(e->dept, dept);
    strcpy(e->title, title);
}

void handle_delete(char *line) {
    char id[MAX_ID];
    if (sscanf(line, "DELETE %63s", id) != 1) return;
    Employee *e = head, *prev = NULL;
    while (e) {
        if (strcmp(e->id, id) == 0) {
            if (prev) prev->next = e->next;
            else head = e->next;
            free(e);
            return;
        }
        prev = e;
        e = e->next;
    }
}

void handle_find(char *line) {
    char id[MAX_ID];
    if (sscanf(line, "FIND %63s", id) != 1) return;
    Employee *e = find_employee(id);
    if (!e) return;
    printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
}

void handle_list() {
    Employee *e = head;
    while (e) {
        printf("%s %s %s %s\n", e->id, e->name, e->dept, e->title);
        e = e->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "UPDATE ", 7) == 0) handle_update(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "LIST") == 0) handle_list();
    }
    free_all();
    return 0;
}