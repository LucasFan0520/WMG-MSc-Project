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
} CSVContact;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    CSVContact *contacts = NULL;
    size_t count = 0;
    size_t cap = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        while (1) {
            char *fline = read_line(fp);
            if (!fline) break;
            char *c1 = strchr(fline, ',');
            if (c1) {
                *c1 = '\0';
                char *email = c1 + 1;
                char *c2 = strchr(email, ',');
                if (c2) {
                    *c2 = '\0';
                    char *phone = c2 + 1;
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        CSVContact *nb = realloc(contacts, cap * sizeof(CSVContact));
                        if (!nb) {
                            free(fline);
                            break;
                        }
                        contacts = nb;
                    }
                    contacts[count].name = safe_dup(fline);
                    contacts[count].email = safe_dup(email);
                    contacts[count].phone = safe_dup(phone);
                    count++;
                }
            }
            free(fline);
        }
        fclose(fp);
    }
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *c1 = strchr(args, ',');
            if (c1) {
                *c1 = '\0';
                char *email = c1 + 1;
                char *c2 = strchr(email, ',');
                if (c2) {
                    *c2 = '\0';
                    char *phone = c2 + 1;
                    if (count >= cap) {
                        cap = cap == 0 ? 4 : cap * 2;
                        CSVContact *nb = realloc(contacts, cap * sizeof(CSVContact));
                        if (!nb) {
                            free(line);
                            break;
                        }
                        contacts = nb;
                    }
                    contacts[count].name = safe_dup(args);
                    contacts[count].email = safe_dup(email);
                    contacts[count].phone = safe_dup(phone);
                    count++;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
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
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    break;
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
