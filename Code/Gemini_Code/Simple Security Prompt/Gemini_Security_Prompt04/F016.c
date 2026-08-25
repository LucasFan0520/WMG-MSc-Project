// F016.c
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
} ContactCSV;

void parse_and_add(char *line, ContactCSV **contacts, int *count, int *cap) {
    char *c1 = strchr(line, ',');
    if (!c1) return;
    char *c2 = strchr(c1 + 1, ',');
    if (!c2) return;
    *c1 = '\0';
    *c2 = '\0';
    char *name = line;
    char *email = c1 + 1;
    char *phone = c2 + 1;
    if (*count >= *cap) {
        *cap = *cap == 0 ? 16 : *cap * 2;
        *contacts = realloc(*contacts, *cap * sizeof(ContactCSV));
    }
    (*contacts)[*count].name = strdup(name);
    (*contacts)[*count].email = strdup(email);
    (*contacts)[*count].phone = strdup(phone);
    (*count)++;
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (!fp) return 1;
    ContactCSV *contacts = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *line = read_line(fp);
        if (!line) break;
        parse_and_add(line, &contacts, &count, &cap);
        free(line);
    }
    fclose(fp);
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            parse_and_add(line + 4, &contacts, &count, &cap);
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
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
