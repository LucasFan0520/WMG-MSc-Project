// F016.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *email;
    char *phone;
} CSVContact;

CSVContact *contacts = NULL;
int count = 0;
int capacity = 0;

void add_contact_str(const char *line) {
    char *line_copy = strdup(line);
    char *name = strtok(line_copy, ",");
    char *email = strtok(NULL, ",");
    char *phone = strtok(NULL, ",");
    if (name && email && phone) {
        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            contacts = realloc(contacts, capacity * sizeof(CSVContact));
        }
        contacts[count].name = strdup(name);
        contacts[count].email = strdup(email);
        contacts[count].phone = strdup(phone);
        count++;
    }
    free(line_copy);
}

void delete_contact(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            free(contacts[i].name);
            free(contacts[i].email);
            free(contacts[i].phone);
            for (int j = i; j < count - 1; j++) {
                contacts[j] = contacts[j + 1];
            }
            count--;
            i--;
        }
    }
}

void find_contact(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(contacts[i].name, name) == 0) {
            printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
        }
    }
}

void report_contacts(void) {
    for (int i = 0; i < count; i++) {
        printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (fp) {
            char *file_line = NULL;
            size_t file_len = 0;
            ssize_t file_read;
            while ((file_read = getline(&file_line, &file_len, fp)) != -1) {
                if (file_read > 0 && file_line[file_read - 1] == '\n') {
                    file_line[file_read - 1] = '\0';
                }
                add_contact_str(file_line);
            }
            free(file_line);
            fclose(fp);
        }
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *arg = strtok(NULL, "");
            if (arg) add_contact_str(arg);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, "");
            if (name) delete_contact(name);
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = strtok(NULL, "");
            if (name) find_contact(name);
        } else if (strcmp(cmd, "REPORT") == 0) {
            report_contacts();
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
