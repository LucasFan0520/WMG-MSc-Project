// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* id;
    char* text;
} Paragraph;

Paragraph* paragraphs = NULL;
size_t p_count = 0;
size_t p_cap = 0;

void cleanup() {
    for (size_t i = 0; i < p_count; i++) {
        free(paragraphs[i].id);
        free(paragraphs[i].text);
    }
    free(paragraphs);
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
        
        if (strcmp(cmd, "NEW") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* id = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* text = ptr;
            
            if (p_count == p_cap) {
                size_t new_cap = p_cap == 0 ? 4 : p_cap * 2;
                Paragraph* temp = realloc(paragraphs, new_cap * sizeof(Paragraph));
                if (!temp) { cleanup(); free(line); return 1; }
                paragraphs = temp;
                p_cap = new_cap;
            }
            paragraphs[p_count].id = strdup(id);
            paragraphs[p_count].text = strdup(text);
            if (!paragraphs[p_count].id || !paragraphs[p_count].text) { cleanup(); free(line); return 1; }
            p_count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* id = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* text = ptr;
            
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(paragraphs[i].id, id) == 0) {
                    size_t old_len = strlen(paragraphs[i].text);
                    size_t add_len = strlen(text);
                    size_t new_len = old_len + 1 + add_len;
                    char* temp = realloc(paragraphs[i].text, new_len + 1);
                    if (!temp) { cleanup(); free(line); return 1; }
                    paragraphs[i].text = temp;
                    paragraphs[i].text[old_len] = ' ';
                    memcpy(paragraphs[i].text + old_len + 1, text, add_len + 1);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* id = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) {
                *ptr = '\0';
                ptr++;
            }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* text = ptr;
            
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(paragraphs[i].id, id) == 0) {
                    char* temp = strdup(text);
                    if (!temp) { cleanup(); free(line); return 1; }
                    free(paragraphs[i].text);
                    paragraphs[i].text = temp;
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* id = ptr;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(paragraphs[i].id, id) == 0) {
                    free(paragraphs[i].id);
                    free(paragraphs[i].text);
                    for (size_t j = i; j < p_count - 1; j++) {
                        paragraphs[j] = paragraphs[j + 1];
                    }
                    p_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* id = ptr;
            int found = 0;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(paragraphs[i].id, id) == 0) {
                    found = 1;
                    for (size_t j = 0; paragraphs[i].text[j]; j++) {
                        if (paragraphs[i].text[j] == ' ') putchar('_');
                        else putchar(paragraphs[i].text[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
            if (!found) printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < p_count; i++) {
                printf("%s\n", paragraphs[i].id);
            }
        }
    }
    
    free(line);
    cleanup();
    return 0;
}