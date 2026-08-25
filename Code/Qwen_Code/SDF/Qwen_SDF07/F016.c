// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line_file(FILE *f) {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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

typedef struct { char *name; char *email; char *phone; } Contact;
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
    if (!contacts[cc].name || !contacts[cc].email || !contacts[cc].phone) { perror("strdup"); exit(1); }
    cc++;
}

void del_contact(const char *name) {
    for (size_t i = 0; i < cc; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            free(contacts[i].name); free(contacts[i].email); free(contacts[i].phone);
            contacts[i] = contacts[--cc];
            return;
        }
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line_file(f)) != NULL) {
        char name[256], email[256], phone[256];
        if (sscanf(line, "%255[^,],%255[^,],%255s", name, email, phone) == 3) {
            add_contact(name, email, phone);
        }
        free(line);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(line, "%*s %255[^,],%255[^,],%255s", name, email, phone) == 3) add_contact(name, email, phone);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                for (size_t i = 0; i < cc; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < cc; i++) printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
        }
        free(line);
    }
    for (size_t i = 0; i < cc; i++) { free(contacts[i].name); free(contacts[i].email); free(contacts[i].phone); }
    free(contacts);
    return 0;
}