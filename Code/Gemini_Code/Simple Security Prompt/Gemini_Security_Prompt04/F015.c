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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *email;
    char *phone;
    char *note;
} Contact;

int main(void) {
    Contact *contacts = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                char *email = p;
                while (*p && *p != ' ') p++;
                if (*p == ' ') {
                    *p = '\0';
                    char *phone = p + 1;
                    while (*phone == ' ') phone++;
                    char *phone_end = phone;
                    while (*phone_end && *phone_end != ' ') phone_end++;
                    if (*phone_end == ' ') *phone_end = '\0';
                    if (count >= cap) {
                        cap = cap == 0 ? 16 : cap * 2;
                        contacts = realloc(contacts, cap * sizeof(Contact));
                    }
                    contacts[count].name = strdup(name);
                    contacts[count].email = strdup(email);
                    contacts[count].phone = strdup(phone);
                    contacts[count].note = strdup("");
                    count++;
                }
            }
        } else if (strncmp(line, "NOTE ", 5) == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *text = p + 1;
                while (*text == ' ') text++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, name) == 0) {
                        free(contacts[i].note);
                        contacts[i].note = strdup(text);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "MERGE ", 6) == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *keep = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *remove = p + 1;
                while (*remove == ' ') remove++;
                char *rem_end = remove;
                while (*rem_end && *rem_end != ' ') rem_end++;
                if (*rem_end == ' ') *rem_end = '\0';
                int idxK = -1, idxR = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(contacts[i].name, keep) == 0) idxK = i;
                    if (strcmp(contacts[i].name, remove) == 0) idxR = i;
                }
                if (idxK != -1 && idxR != -1) {
                    size_t l1 = strlen(contacts[idxK].note);
                    size_t l2 = strlen(contacts[idxR].note);
                    char *nn = malloc(l1 + l2 + 2);
                    strcpy(nn, contacts[idxK].note);
                    if (l1 > 0 && l2 > 0) strcat(nn, " ");
                    strcat(nn, contacts[idxR].note);
                    free(contacts[idxK].note);
                    contacts[idxK].note = nn;
                    free(contacts[idxR].name);
                    free(contacts[idxR].email);
                    free(contacts[idxR].phone);
                    free(contacts[idxR].note);
                    for (int i = idxR; i < count - 1; i++) {
                        contacts[i] = contacts[i + 1];
                    }
                    count--;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
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
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            while (*name == ' ') name++;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
                    break;
                }
            }
        } else if (strcmp(line, "LIST") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
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
