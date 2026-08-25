// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_EMAIL 128
#define MAX_PHONE 32

typedef struct Contact {
    char name[MAX_NAME];
    char email[MAX_EMAIL];
    char phone[MAX_PHONE];
    struct Contact *next;
} Contact;

Contact *head = NULL;

Contact *find_contact(const char *name) {
    Contact *c = head;
    while (c) {
        if (strcmp(c->name, name) == 0) return c;
        c = c->next;
    }
    return NULL;
}

void free_all() {
    Contact *c = head;
    while (c) {
        Contact *tmp = c;
        c = c->next;
        free(tmp);
    }
    head = NULL;
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char name[MAX_NAME], email[MAX_EMAIL], phone[MAX_PHONE];
        if (sscanf(line, "%63[^,],%127[^,],%31s", name, email, phone) == 3) {
            if (find_contact(name)) continue;
            Contact *c = malloc(sizeof(Contact));
            if (!c) continue;
            strcpy(c->name, name);
            strcpy(c->email, email);
            strcpy(c->phone, phone);
            c->next = head;
            head = c;
        }
    }
    fclose(f);
}

void handle_add(char *line) {
    char name[MAX_NAME], email[MAX_EMAIL], phone[MAX_PHONE];
    if (sscanf(line, "ADD %63[^,],%127[^,],%31s", name, email, phone) != 3) return;
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    if (!c) return;
    strcpy(c->name, name);
    strcpy(c->email, email);
    strcpy(c->phone, phone);
    c->next = head;
    head = c;
}

void handle_delete(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DELETE %63s", name) != 1) return;
    Contact *c = head, *prev = NULL;
    while (c) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
}

void handle_find(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "FIND %63s", name) != 1) return;
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

void handle_report() {
    Contact *c = head;
    while (c) {
        printf("%s,%s,%s\n", c->name, c->email, c->phone);
        c = c->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "REPORT") == 0) handle_report();
    }
    free_all();
    return 0;
}