// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* name;
    char* note;
    char** labels;
    size_t l_count;
    size_t l_cap;
} Item;

Item* items = NULL;
size_t i_count = 0;
size_t i_cap = 0;

void cleanup() {
    for (size_t i = 0; i < i_count; i++) {
        for (size_t j = 0; j < items[i].l_count; j++) {
            free(items[i].labels[j]);
        }
        free(items[i].labels);
        free(items[i].name);
        free(items[i].note);
    }
    free(items);
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
        
        if (strcmp(cmd, "ITEM") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* note = ptr;
            
            if (i_count == i_cap) {
                size_t new_cap = i_cap == 0 ? 4 : i_cap * 2;
                Item* temp = realloc(items, new_cap * sizeof(Item));
                if (!temp) { cleanup(); free(line); return 1; }
                items = temp;
                i_cap = new_cap;
            }
            items[i_count].name = strdup(name);
            items[i_count].note = strdup(note);
            items[i_count].labels = NULL;
            items[i_count].l_count = 0;
            items[i_count].l_cap = 0;
            if (!items[i_count].name || !items[i_count].note) { cleanup(); free(line); return 1; }
            i_count++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* label = ptr;
            
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (items[i].l_count == items[i].l_cap) {
                        size_t new_cap = items[i].l_cap == 0 ? 4 : items[i].l_cap * 2;
                        char** temp = realloc(items[i].labels, new_cap * sizeof(char*));
                        if (!temp) { cleanup(); free(line); return 1; }
                        items[i].labels = temp;
                        items[i].l_cap = new_cap;
                    }
                    items[i].labels[items[i].l_count] = strdup(label);
                    if (!items[i].labels[items[i].l_count]) { cleanup(); free(line); return 1; }
                    items[i].l_count++;
                    break;
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* label = ptr;
            
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    for (size_t j = 0; j < items[i].l_count; j++) {
                        if (strcmp(items[i].labels[j], label) == 0) {
                            free(items[i].labels[j]);
                            for (size_t k = j; k < items[i].l_count - 1; k++) {
                                items[i].labels[k] = items[i].labels[k + 1];
                            }
                            items[i].l_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    for (size_t j = 0; j < items[i].l_count; j++) {
                        free(items[i].labels[j]);
                    }
                    free(items[i].labels);
                    free(items[i].name);
                    free(items[i].note);
                    for (size_t j = i; j < i_count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    i_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            for (size_t i = 0; i < i_count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    printf("%s %s", items[i].name, items[i].note);
                    for (size_t j = 0; j < items[i].l_count; j++) {
                        printf(" %s", items[i].labels[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < i_count; i++) {
                printf("%s %s", items[i].name, items[i].note);
                for (size_t j = 0; j < items[i].l_count; j++) {
                    printf(" %s", items[i].labels[j]);
                }
                printf("\n");
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}