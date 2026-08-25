// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main(void) {
    Contact *contacts = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            char *email = p;
            while (*p && *p != ' ') p++;
            char *phone = "";
            if (*p == ' ') {
                *p = '\0';
                phone = p + 1;
            }
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                Contact *nb = realloc(contacts, cap * sizeof(Contact));
                if (!nb) {
                    free(line);
                    break;
                }
                contacts = nb;
            }
            contacts[count].name = safe_dup(name);
            contacts[count].email = safe_dup(email);
            contacts[count].phone = safe_dup(phone);
            contacts[count].note = safe_dup("");
            count++;
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *p = line + 5;
            char *name = p;
            while (*p && *p != ' ') p++;
            char *note_text = "";
            if (*p == ' ') {
                *p = '\0';
                note_text = p + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].note);
                    contacts[i].note = safe_dup(note_text);
                    break;
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            char *keep = p;
            while (*p && *p != ' ') p++;
            char *remove = "";
            if (*p == ' ') {
                *p = '\0';
                remove = p + 1;
            }
            size_t idx_keep = (size_t)-1;
            size_t idx_rem = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, keep) == 0) idx_keep = i;
                if (strcmp(contacts[i].name, remove) == 0) idx_rem = i;
            }
            if (idx_keep != (size_t)-1 && idx_rem != (size_t)-1) {
                size_t l1 = strlen(contacts[idx_keep].note);
                size_t l2 = strlen(contacts[idx_rem].note);
                char *nn = malloc(l1 + l2 + 2);
                if (nn) {
                    strcpy(nn, contacts[idx_keep].note);
                    if (l1 > 0 && l2 > 0) {
                        strcat(nn, " ");
                        strcat(nn, contacts[idx_rem].note);
                    } else if (l2 > 0) {
                        strcpy(nn, contacts[idx_rem].note);
                    }
                    free(contacts[idx_keep].note);
                    contacts[idx_keep].note = nn;
                }
                free(contacts[idx_rem].name);
                free(contacts[idx_rem].email);
                free(contacts[idx_rem].phone);
                free(contacts[idx_rem].note);
                for (size_t j = idx_rem; j < count - 1; j++) {
                    contacts[j] = contacts[j + 1];
                }
                count--;
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    free(contacts[i].note);
                    for (size_t j = i; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
    return 0;
}
