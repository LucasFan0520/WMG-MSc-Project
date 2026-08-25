// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char** lines = NULL;
size_t l_count = 0;
size_t l_cap = 0;

void cleanup() {
    for (size_t i = 0; i < l_count; i++) {
        free(lines[i]);
    }
    free(lines);
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
        
        if (strcmp(cmd, "INSERT") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* idx_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* text = ptr;
            
            long idx = strtol(idx_str, NULL, 10);
            if (idx < 0 || idx > (long)l_count) continue;
            
            if (l_count == l_cap) {
                size_t new_cap = l_cap == 0 ? 4 : l_cap * 2;
                char** temp = realloc(lines, new_cap * sizeof(char*));
                if (!temp) { cleanup(); free(line); return 1; }
                lines = temp;
                l_cap = new_cap;
            }
            
            for (size_t i = l_count; i > (size_t)idx; i--) {
                lines[i] = lines[i - 1];
            }
            lines[idx] = strdup(text);
            if (!lines[idx]) { cleanup(); free(line); return 1; }
            l_count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* text = ptr;
            
            if (l_count == l_cap) {
                size_t new_cap = l_cap == 0 ? 4 : l_cap * 2;
                char** temp = realloc(lines, new_cap * sizeof(char*));
                if (!temp) { cleanup(); free(line); return 1; }
                lines = temp;
                l_cap = new_cap;
            }
            lines[l_count] = strdup(text);
            if (!lines[l_count]) { cleanup(); free(line); return 1; }
            l_count++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* idx_str = ptr;
            long idx = strtol(idx_str, NULL, 10);
            if (idx < 0 || idx >= (long)l_count) continue;
            
            free(lines[idx]);
            for (size_t i = idx; i < l_count - 1; i++) {
                lines[i] = lines[i + 1];
            }
            l_count--;
        } else if (strcmp(cmd, "PATCH") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* idx_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* text = ptr;
            
            long idx = strtol(idx_str, NULL, 10);
            if (idx < 0 || idx >= (long)l_count) continue;
            
            char* temp = strdup(text);
            if (!temp) { cleanup(); free(line); return 1; }
            free(lines[idx]);
            lines[idx] = temp;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < l_count; i++) {
                for (size_t j = 0; lines[i][j]; j++) {
                    if (lines[i][j] == ' ') putchar('_');
                    else putchar(lines[i][j]);
                }
                putchar('\n');
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}