// F016.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
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
    if (!contacts[count].name || !contacts[count].email || !contacts[count].phone)
        { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void delete_contact(const char *name) {
    ssize_t idx = find_contact(name);
    if (idx == -1) return;
    free(contacts[idx].name);
    free(contacts[idx].email);
    free(contacts[idx].phone);
    for (size_t i = idx; i < count - 1; i++)
        contacts[i] = contacts[i + 1];
    count--;
}

static void find_contact_print(const char *name) {
    ssize_t idx = find_contact(name);
    if (idx == -1) return;
    printf("%s,%s,%s\n", contacts[idx].name, contacts[idx].email, contacts[idx].phone);
}

static void report(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
}

static void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, f)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        char *p = line;
        char *name = strsep(&p, ",");
        char *email = strsep(&p, ",");
        char *phone = strsep(&p, ",");
        if (name && email && phone && p == NULL) {
            add_contact(name, email, phone);
        }
    }
    free(line);
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);

    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], arg[512];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s", arg) == 1) {
                char *p = arg;
                char *name = strsep(&p, ",");
                char *email = strsep(&p, ",");
                char *phone = strsep(&p, ",");
                if (name && email && phone && p == NULL)
                    add_contact(name, email, phone);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                find_contact_print(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }

    free(line);
    free_all();
    return 0;
}