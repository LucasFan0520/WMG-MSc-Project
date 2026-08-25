// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* name;
    char* email;
    char* phone;
} Contact;

Contact* contacts = NULL;
size_t c_count = 0;
size_t c_cap = 0;

void cleanup() {
    for (size_t i = 0; i < c_count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
    }
    free(contacts);
}

void add_contact(const char* name, const char* email, const char* phone) {
    if (c_count == c_cap) {
        size_t new_cap = c_cap == 0 ? 4 : c_cap * 2;
        Contact* temp = realloc(contacts, new_cap * sizeof(Contact));
        if (!temp) exit(1);
        contacts = temp;
        c_cap = new_cap;
    }
    contacts[c_count].name = strdup(name);
    contacts[c_count].email = strdup(email);
    contacts[c_count].phone = strdup(phone);
    if (!contacts[c_count].name || !contacts[c_count].email || !contacts[c_count].phone) exit(1);
    c_count++;
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    
    FILE* f = fopen(argv[1], "r");
    if (!f) return 1;
    
    char* line = NULL;
    size_t len = 0;
    while (getline(&line, &len, f) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        char* name = strtok(line, ",");
        char* email = strtok(NULL, ",");
        char* phone = strtok(NULL, ",");
        if (name && email && phone) {
            add_contact(name, email, phone);
        }
    }
    fclose(f);
    free(line);
    
    line = NULL;
    len = 0;
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char* ptr = line;
        while (*ptr && isspace((unsigned char)*ptr)) ptr++;
        if (!*ptr) continue;
        
        char* cmd = ptr;
        while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }
        
        if (strcmp(cmd, "ADD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* data = ptr;
            char* name = strtok(data, ",");
            char* email = strtok(NULL, ",");
            char* phone = strtok(NULL, ",");
            if (name && email && phone) {
                add_contact(name, email, phone);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    for (size_t j = i; j < c_count - 1; j++) {
                        contacts[j] = contacts[j + 1];
                    }
                    c_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s,%s,%s\n", contacts[i].name, contacts[i].email, contacts[i].phone);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}