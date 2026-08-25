// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* title;
    char* body;
} Draft;

Draft* drafts = NULL;
size_t d_count = 0;
size_t d_cap = 0;

Draft* undo_stack = NULL;
size_t u_count = 0;
size_t u_cap = 0;

void cleanup() {
    for (size_t i = 0; i < d_count; i++) {
        free(drafts[i].title);
        free(drafts[i].body);
    }
    free(drafts);
    for (size_t i = 0; i < u_count; i++) {
        free(undo_stack[i].title);
        free(undo_stack[i].body);
    }
    free(undo_stack);
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
        
        if (strcmp(cmd, "CREATE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* body = ptr;
            
            if (d_count == d_cap) {
                size_t new_cap = d_cap == 0 ? 4 : d_cap * 2;
                Draft* temp = realloc(drafts, new_cap * sizeof(Draft));
                if (!temp) { cleanup(); free(line); return 1; }
                drafts = temp;
                d_cap = new_cap;
            }
            drafts[d_count].title = strdup(title);
            drafts[d_count].body = strdup(body);
            if (!drafts[d_count].title || !drafts[d_count].body) { cleanup(); free(line); return 1; }
            d_count++;
        } else if (strcmp(cmd, "REVISE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* body = ptr;
            
            for (size_t i = 0; i < d_count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    char* temp = strdup(body);
                    if (!temp) { cleanup(); free(line); return 1; }
                    free(drafts[i].body);
                    drafts[i].body = temp;
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            for (size_t i = 0; i < d_count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    if (u_count == u_cap) {
                        size_t new_cap = u_cap == 0 ? 4 : u_cap * 2;
                        Draft* temp = realloc(undo_stack, new_cap * sizeof(Draft));
                        if (!temp) { cleanup(); free(line); return 1; }
                        undo_stack = temp;
                        u_cap = new_cap;
                    }
                    undo_stack[u_count].title = drafts[i].title;
                    undo_stack[u_count].body = drafts[i].body;
                    u_count++;
                    
                    for (size_t j = i; j < d_count - 1; j++) {
                        drafts[j] = drafts[j + 1];
                    }
                    d_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (u_count > 0) {
                u_count--;
                if (d_count == d_cap) {
                    size_t new_cap = d_cap == 0 ? 4 : d_cap * 2;
                    Draft* temp = realloc(drafts, new_cap * sizeof(Draft));
                    if (!temp) { cleanup(); free(line); return 1; }
                    drafts = temp;
                    d_cap = new_cap;
                }
                drafts[d_count] = undo_stack[u_count];
                d_count++;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* title = ptr;
            for (size_t i = 0; i < d_count; i++) {
                if (strcmp(drafts[i].title, title) == 0) {
                    printf("%s %s\n", drafts[i].title, drafts[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < d_count; i++) {
                printf("%s %s\n", drafts[i].title, drafts[i].body);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}