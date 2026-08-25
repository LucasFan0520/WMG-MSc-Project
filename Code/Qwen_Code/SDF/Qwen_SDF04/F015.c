// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* name;
    char* email;
    char* phone;
    char* note;
} Contact;

Contact* contacts = NULL;
size_t c_count = 0;
size_t c_cap = 0;

void cleanup() {
    for (size_t i = 0; i < c_count; i++) {
        free(contacts[i].name);
        free(contacts[i].email);
        free(contacts[i].phone);
        free(contacts[i].note);
    }
    free(contacts);
}

int main() {
    char* line = NULL;
    size_t len = 0;
    
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
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* email = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* phone = ptr;
            
            if (c_count == c_cap) {
                size_t new_cap = c_cap == 0 ? 4 : c_cap * 2;
                Contact* temp = realloc(contacts, new_cap * sizeof(Contact));
                if (!temp) { cleanup(); free(line); return 1; }
                contacts = temp;
                c_cap = new_cap;
            }
            contacts[c_count].name = strdup(name);
            contacts[c_count].email = strdup(email);
            contacts[c_count].phone = strdup(phone);
            contacts[c_count].note = strdup("");
            if (!contacts[c_count].name || !contacts[c_count].email || !contacts[c_count].phone || !contacts[c_count].note) { cleanup(); free(line); return 1; }
            c_count++;
        } else if (strcmp(cmd, "NOTE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* text = ptr;
            
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    char* temp = strdup(text);
                    if (!temp) { cleanup(); free(line); return 1; }
                    free(contacts[i].note);
                    contacts[i].note = temp;
                    break;
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* keep = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* remove = ptr;
            
            Contact* k = NULL;
            Contact* r = NULL;
            size_t k_idx = 0, r_idx = 0;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(contacts[i].name, keep) == 0) { k = &contacts[i]; k_idx = i; }
                if (strcmp(contacts[i].name, remove) == 0) { r = &contacts[i]; r_idx = i; }
            }
            
            if (k && r) {
                size_t old_len = strlen(k->note);
                size_t add_len = strlen(r->note);
                size_t total = old_len + (old_len > 0 && add_len > 0 ? 1 : 0) + add_len;
                char* temp = malloc(total + 1);
                if (!temp) { cleanup(); free(line); return 1; }
                if (old_len > 0 && add_len > 0) {
                    snprintf(temp, total + 1, "%s %s", k->note, r->note);
                } else if (old_len > 0) {
                    strcpy(temp, k->note);
                } else {
                    strcpy(temp, r->note);
                }
                free(k->note);
                k->note = temp;
                
                free(r->name);
                free(r->email);
                free(r->phone);
                free(r->note);
                for (size_t i = r_idx; i < c_count - 1; i++) {
                    contacts[i] = contacts[i + 1];
                }
                c_count--;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            for (size_t i = 0; i < c_count; i++) {
                if (strcmp(contacts[i].name, name) == 0) {
                    free(contacts[i].name);
                    free(contacts[i].email);
                    free(contacts[i].phone);
                    free(contacts[i].note);
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
                    printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < c_count; i++) {
                printf("%s %s %s %s\n", contacts[i].name, contacts[i].email, contacts[i].phone, contacts[i].note);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}