// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *name; char *email; char *phone; char *note; } Contact;
Contact *contacts = NULL;
size_t cc = 0, ccap = 0;

void add_contact(const char *name, const char *email, const char *phone) {
    for (size_t i = 0; i < cc; i++) {
        if (strcmp(contacts[i].name, name) == 0) return;
    }
    if (cc == ccap) {
        size_t nc = ccap ? ccap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Contact)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Contact *tmp = realloc(contacts, nc * sizeof(Contact));
        if (!tmp) { perror("realloc"); exit(1); }
        contacts = tmp;
        ccap = nc;
    }
    contacts[cc].name = strdup(name);
    contacts[cc].email = strdup(email);
    contacts[cc].phone = strdup(phone);
    contacts[cc].note = strdup("");
    if (!contacts[cc].name || !contacts[cc].email || !contacts[cc].phone || !contacts[cc].note) { perror("strdup"); exit(1); }
    cc++;
}

void note_contact(const char *name, const char *text) {
    for (size_t i = 0; i < cc; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            char *tmp = strdup(text);
            if (!tmp) { perror("strdup"); exit(1); }
            free(contacts[i].note);
            contacts[i].note = tmp;
            return;
        }
    }
}

void merge_contact(const char *keep, const char *remove) {
    Contact *k = NULL, *r = NULL;
    for (size_t i = 0; i < cc; i++) {
        if (strcmp(contacts[i].name, keep) == 0) k = &contacts[i];
        if (strcmp(contacts[i].name, remove) == 0) r = &contacts[i];
    }
    if (!k || !r) return;
    size_t l1 = strlen(k->note), l2 = strlen(r->note);
    if (l1 + l2 + 2 < l1) { fprintf(stderr, "Overflow\n"); exit(1); }
    char *tmp = malloc(l1 + l2 + 2);
    if (!tmp) { perror("malloc"); exit(1); }
    snprintf(tmp, l1 + l2 + 2, "%s %s", k->note, r->note);
    free(k->note);
    k->note = tmp;
    free(r->name); free(r->email); free(r->phone); free(r->note);
    for (size_t i = 0; i < cc; i++) {
        if (&contacts[i] == r) {
            contacts[i] = contacts[--cc];
            break;
        }
    }
}

void del_contact(const char *name) {
    for (size_t i = 0; i < cc; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            free(contacts[i].name); free(contacts[i].email); free(contacts[i].phone); free(contacts[i].note);
            contacts[i] = contacts[--cc];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(line, "%*s %255s %255s %255s", name, email, phone) == 3) add_contact(name, email, phone);
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) != 1) { free(line); continue; }
            char *text = strchr(line, ' ');
            if (text) { text = strchr(text + 1, ' '); if (text) text++; }
            if (!text) text = "";
            note_contact(name, text);
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[256], remove[256];
            if (sscanf(line, "%*s %255s %255s", keep, remove) == 2) merge_contact(keep, remove);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                for (size_t i = 0; i < cc; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < cc; i++) printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
        }
        free(line);
    }
    for (size_t i = 0; i < cc; i++) { free(contacts[i].name); free(contacts[i].email); free(contacts[i].phone); free(contacts[i].note); }
    free(contacts);
    return 0;
}