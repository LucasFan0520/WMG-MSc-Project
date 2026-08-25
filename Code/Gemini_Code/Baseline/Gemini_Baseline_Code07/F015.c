// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 1024;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) { free(buf); return NULL; }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main() {
    Contact *contacts = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char cmd[64];
        int n = 0;
        if (sscanf(line, "%63s%n", cmd, &n) < 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note ? contacts[i].note : "");
            }
            free(line);
            continue;
        }
        char *p = line + n;
        while (*p == ' ' || *p == '\t') p++;
        if (strcmp(cmd, "ADD") == 0) {
            char name[256], email[256], phone[256];
            if (sscanf(p, "%255s %255s %255s", name, email, phone) == 3) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    contacts = realloc(contacts, capacity * sizeof(Contact));
                }
                contacts[count].name = strdup(name);
                contacts[count].email = strdup(email);
                contacts[count].phone = strdup(phone);
                contacts[count].note = NULL;
                count++;
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char name[256];
            int n2 = 0;
            if (sscanf(p, "%255s%n", name, &n2) == 1) {
                p += n2;
                while (*p == ' ' || *p == '\t') p++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        free(contacts[i].note);
                        contacts[i].note = strdup(p);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char keep[256], remove[256];
            if (sscanf(p, "%255s %255s", keep, remove) == 2) {
                int fKeep = -1, fRemove = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, keep) == 0) fKeep = i;
                    if (strcmp(contacts[i].name, remove) == 0) fRemove = i;
                }
                if (fKeep != -1 && fRemove != -1) {
                    if (contacts[fRemove].note && strlen(contacts[fRemove].note) > 0) {
                        if (contacts[fKeep].note && strlen(contacts[fKeep].note) > 0) {
                            size_t l1 = strlen(contacts[fKeep].note);
                            size_t l2 = strlen(contacts[fRemove].note);
                            char *nn = malloc(l1 + l2 + 2);
                            strcpy(nn, contacts[fKeep].note);
                            strcat(nn, " ");
                            strcat(nn, contacts[fRemove].note);
                            free(contacts[fKeep].note);
                            contacts[fKeep].note = nn;
                        } else {
                            contacts[fKeep].note = strdup(contacts[fRemove].note);
                        }
                    }
                    free(contacts[fRemove].name);
                    free(contacts[fRemove].email);
                    free(contacts[fRemove].phone);
                    free(contacts[fRemove].note);
                    for (int i = fRemove; i < count - 1; i++) {
                        contacts[i] = contacts[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(contacts[found].name);
                    free(contacts[found].email);
                    free(contacts[found].phone);
                    free(contacts[found].note);
                    for (int i = found; i < count - 1; i++) {
                        contacts[i] = contacts[i + 1];
                    }
                    count--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char name[256];
            if (sscanf(p, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note ? contacts[i].note : "");
                        break;
                    }
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}
