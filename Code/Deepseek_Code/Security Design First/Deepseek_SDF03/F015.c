// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_EMAIL 128
#define MAX_PHONE 32
#define MAX_NOTE 4096

typedef struct Contact {
    char name[MAX_NAME];
    char email[MAX_EMAIL];
    char phone[MAX_PHONE];
    char note[MAX_NOTE];
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

void handle_add(char *line) {
    char name[MAX_NAME], email[MAX_EMAIL], phone[MAX_PHONE];
    if (sscanf(line, "ADD %63s %127s %31s", name, email, phone) != 3) return;
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    if (!c) return;
    strcpy(c->name, name);
    strcpy(c->email, email);
    strcpy(c->phone, phone);
    c->note[0] = '\0';
    c->next = head;
    head = c;
}

void handle_note(char *line) {
    char name[MAX_NAME], note[MAX_NOTE];
    int n = sscanf(line, "NOTE %63s %4095[^\n]", name, note);
    if (n < 2) return;
    Contact *c = find_contact(name);
    if (!c) return;
    strcpy(c->note, note);
}

void handle_merge(char *line) {
    char keep[MAX_NAME], remove[MAX_NAME];
    if (sscanf(line, "MERGE %63s %63s", keep, remove) != 2) return;
    Contact *k = find_contact(keep);
    Contact *r = find_contact(remove);
    if (!k || !r) return;
    // Combine notes: keep's note + space + r's note
    size_t len = strlen(k->note) + strlen(r->note) + 2;
    if (len > MAX_NOTE) return;
    if (k->note[0]) strcat(k->note, " ");
    strcat(k->note, r->note);
    // Delete r
    Contact *c = head, *prev = NULL;
    while (c) {
        if (c == r) {
            if (prev) prev->next = c->next;
            else head = c->next;
            free(c);
            return;
        }
        prev = c;
        c = c->next;
    }
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
    printf("%s %s %s", c->name, c->email, c->phone);
    if (c->note[0]) printf(" %s", c->note);
    putchar('\n');
}

void handle_list() {
    Contact *c = head;
    while (c) {
        printf("%s %s %s", c->name, c->email, c->phone);
        if (c->note[0]) printf(" %s", c->note);
        putchar('\n');
        c = c->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "NOTE ", 5) == 0) handle_note(line);
        else if (strncmp(line, "MERGE ", 6) == 0) handle_merge(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strncmp(line, "FIND ", 5) == 0) handle_find(line);
        else if (strcmp(line, "LIST") == 0) handle_list();
    }
    free_all();
    return 0;
}