// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* name;
    char* expansion;
} Alias;

Alias* aliases = NULL;
size_t a_count = 0;
size_t a_cap = 0;

void cleanup() {
    for (size_t i = 0; i < a_count; i++) {
        free(aliases[i].name);
        free(aliases[i].expansion);
    }
    free(aliases);
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
        
        if (strcmp(cmd, "DEFINE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* exp = ptr;
            
            int found = 0;
            for (size_t i = 0; i < a_count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    found = 1;
                    char* temp = strdup(exp);
                    if (!temp) { cleanup(); free(line); return 1; }
                    free(aliases[i].expansion);
                    aliases[i].expansion = temp;
                    break;
                }
            }
            if (!found) {
                if (a_count == a_cap) {
                    size_t new_cap = a_cap == 0 ? 4 : a_cap * 2;
                    Alias* temp = realloc(aliases, new_cap * sizeof(Alias));
                    if (!temp) { cleanup(); free(line); return 1; }
                    aliases = temp;
                    a_cap = new_cap;
                }
                aliases[a_count].name = strdup(name);
                aliases[a_count].expansion = strdup(exp);
                if (!aliases[a_count].name || !aliases[a_count].expansion) { cleanup(); free(line); return 1; }
                a_count++;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            for (size_t i = 0; i < a_count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    for (size_t j = 0; aliases[i].expansion[j]; j++) {
                        if (aliases[i].expansion[j] == ' ') putchar('_');
                        else putchar(aliases[i].expansion[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* old_name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* new_name = ptr;
            
            for (size_t i = 0; i < a_count; i++) {
                if (strcmp(aliases[i].name, old_name) == 0) {
                    char* temp = strdup(new_name);
                    if (!temp) { cleanup(); free(line); return 1; }
                    free(aliases[i].name);
                    aliases[i].name = temp;
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            for (size_t i = 0; i < a_count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    free(aliases[i].name);
                    free(aliases[i].expansion);
                    for (size_t j = i; j < a_count - 1; j++) {
                        aliases[j] = aliases[j + 1];
                    }
                    a_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < a_count; i++) {
                printf("%s %s\n", aliases[i].name, aliases[i].expansion);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}