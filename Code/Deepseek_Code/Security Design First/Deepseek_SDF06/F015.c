// F015.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

static Contact *contacts = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_contact(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(contacts[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name) != -1) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Contact *new = realloc(contacts, capacity * sizeof(Contact));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        contacts = new;
    }
    contacts[count].name = strdup(name);
    contacts[count].email = strdup(email);
    contacts[count].phone = strdup(phone);
    contacts[count].note = strdup("");
    if (!contacts[count].name || !contacts[count].email || !contacts[count].phone || !contacts[count].note)
        { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void note_contact(const char *name, const char *note) {
    ssize_t idx = find_contact(name);
    if (idx == -1) return;
    free(contacts[idx].note);
    contacts[idx].note = strdup(note);
    if (!contacts[idx].note) { fprintf(stderr, "Memory error\n"); exit(1); }
}

static void merge_contacts(const char *keep, const char *remove) {
    ssize_t ki = find_contact(keep);
    ssize_t ri = find_contact(remove);
    if (ki == -1 || ri == -1) return;
    // combine notes
    size_t new_len = strlen(contacts[ki].note) + 1 + strlen(contacts[ri].note) + 1;
    char *new_note = malloc(new_len);
    if (!new_note) { fprintf(stderr, "Memory error\n"); exit(1); }
    snprintf(new_note, new_len, "%s %s", contacts[ki].note, contacts[ri].note);
    free(contacts[ki].note);
    contacts[ki].note = new_note;
    // delete remove
    free(contacts[ri].name);
    free(contacts[ri].email);
    free(contacts[ri].phone);
    free(contacts[ri].note);
    for (size_t i = ri; i < count - 1; i++)
        contacts[i] = contacts[i + 1];
    count--;
}

static void delete_contact(const char *name) {
    ssize_t idx = find_contact(name);
    if (idx == -1) return;
    free(contacts[idx].name);
    free(contacts[idx].email);
    free(contacts[idx].phone);
    free(contacts[idx].note);
    for (size_t i = idx; i < count - 1; i++)
        contacts[i] = contacts[i + 1];
    count--;
}

static void find_contact_print(const char *name) {
    ssize_t idx = find_contact(name);
    if (idx == -1) return;
    printf("%s %s %s %s\n", contacts[idx].name, contacts[idx].email,
           contacts[idx].phone, contacts[idx].note);
}

static void list_contacts(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s %s %s\n", contacts[i].name, contacts[i].email,
               contacts[i].phone, contacts[i].note);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], arg1[256], arg2[256], note[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %255s %255s", name, arg1, arg2) == 3)
                add_contact(name, arg1, arg2);
        } else if (strcmp(cmd, "NOTE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, note) == 2)
                note_contact(name, note);
        } else if (strcmp(cmd, "MERGE") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, arg1) == 2)
                merge_contacts(name, arg1);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_contact_print(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_contacts();
        }
    }

    free(line);
    free_all();
    return 0;
}