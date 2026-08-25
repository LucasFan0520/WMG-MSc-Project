// filename: F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Contact {
    char *name;
    char *email;
    char *phone;
    struct Contact *next;
} Contact;

static Contact *contacts = NULL;

static void cleanup(void) {
    while (contacts) {
        Contact *tmp = contacts;
        contacts = contacts->next;
        free(tmp->name);
        free(tmp->email);
        free(tmp->phone);
        free(tmp);
    }
}

static void load_csv(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        line[strcspn(line, "\n")] = '\0';
        char *name = strtok(line, ",");
        char *email = strtok(NULL, ",");
        char *phone = strtok(NULL, ",");
        if (name && email && phone) {
            Contact *c = malloc(sizeof(Contact));
            if (!c) continue;
            c->name = strdup(name);
            c->email = strdup(email);
            c->phone = strdup(phone);
            if (!c->name || !c->email || !c->phone) {
                free(c->name);
                free(c->email);
                free(c->phone);
                free(c);
                continue;
            }
            c->next = contacts;
            contacts = c;
        }
    }
    free(line);
    fclose(f);
}

static Contact *find_contact(const char *name) {
    for (Contact *c = contacts; c; c = c->next)
        if (strcmp(c->name, name) == 0)
            return c;
    return NULL;
}

static void add_contact(const char *name, const char *email, const char *phone) {
    if (find_contact(name)) return;
    Contact *c = malloc(sizeof(Contact));
    if (!c) return;
    c->name = strdup(name);
    c->email = strdup(email);
    c->phone = strdup(phone);
    if (!c->name || !c->email || !c->phone) {
        free(c->name);
        free(c->email);
        free(c->phone);
        free(c);
        return;
    }
    c->next = contacts;
    contacts = c;
}

static void delete_contact(const char *name) {
    Contact *prev = NULL;
    for (Contact *c = contacts; c; c = c->next) {
        if (strcmp(c->name, name) == 0) {
            if (prev) prev->next = c->next;
            else contacts = c->next;
            free(c->name);
            free(c->email);
            free(c->phone);
            free(c);
            return;
        }
        prev = c;
    }
}

static void find_print(const char *name) {
    Contact *c = find_contact(name);
    if (!c) return;
    printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

static void report(void) {
    for (Contact *c = contacts; c; c = c->next)
        printf("%s,%s,%s\n", c->name, c->email, c->phone);
}

int main(int argc, char **argv) {
    if (argc > 1) load_csv(argv[1]);
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg3 = p;
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1 && *arg2) add_contact(arg1, arg2, arg3);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_contact(arg1);
        } else if (strcmp(cmd, "FIND") == 0) {
            if (*arg1) find_print(arg1);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report();
        }
    }
    free(line);
    return 0;
}