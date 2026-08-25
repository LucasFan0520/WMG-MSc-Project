/* F016.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} CSVContact;

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    CSVContact *contacts = NULL;
    int count = 0;
    int capacity = 0;

    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        char *fline = NULL;
        size_t flen = 0;
        ssize_t fread_bytes;
        while ((fread_bytes = getline(&fline, &flen, fp)) != -1) {
            fline[strcspn(fline, "\r\n")] = '\0';
            char *c1 = strchr(fline, ',');
            if (!c1) continue;
            char *c2 = strchr(c1 + 1, ',');
            if (!c2) continue;
            *c1 = '\0';
            *c2 = '\0';
            char *name = fline;
            char *email = c1 + 1;
            char *phone = c2 + 1;
            char *n_name = strdup(name);
            char *n_email = strdup(email);
            char *n_phone = strdup(phone);
            if (n_name && n_email && n_phone) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    CSVContact *tmp = realloc(contacts, new_cap * sizeof(CSVContact));
                    if (tmp) {
                        contacts = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_name); free(n_email); free(n_phone);
                        continue;
                    }
                }
                contacts[count].name = n_name;
                contacts[count].email = n_email;
                contacts[count].phone = n_phone;
                count++;
            } else {
                free(n_name); free(n_email); free(n_phone);
            }
        }
        free(fline);
        fclose(fp);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *args = line + 4;
            char *c1 = strchr(args, ',');
            if (!c1) continue;
            char *c2 = strchr(c1 + 1, ',');
            if (!c2) continue;
            *c1 = '\0';
            *c2 = '\0';
            char *name = args;
            char *email = c1 + 1;
            char *phone = c2 + 1;
            char *n_name = strdup(name);
            char *n_email = strdup(email);
            char *n_phone = strdup(phone);
            if (n_name && n_email && n_phone) {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    CSVContact *tmp = realloc(contacts, new_cap * sizeof(CSVContact));
                    if (tmp) {
                        contacts = tmp;
                        capacity = new_cap;
                    } else {
                        free(n_name); free(n_email); free(n_phone);
                        continue;
                    }
                }
                contacts[count].name = n_name;
                contacts[count].email = n_email;
                contacts[count].phone = n_phone;
                count++;
            } else {
                free(n_name); free(n_email); free(n_phone);
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *name = line + 7;
            int i = 0;
            while (i < count) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    for (int j = i; j < count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    count--;
                } else {
                    i++;
                }
            }
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *name = line + 5;
            for (int i = 0; i < count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                }
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
    return 0;
}
