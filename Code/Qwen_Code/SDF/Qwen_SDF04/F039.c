// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* name; int rows, cols; char*** cells; } Table;

Table* tables = NULL;
size_t t_count = 0, t_cap = 0;

void cleanup() {
    for (size_t i = 0; i < t_count; i++) {
        for (int r = 0; r < tables[i].rows; r++) {
            for (int c = 0; c < tables[i].cols; c++) {
                free(tables[i].cells[r][c]);
            }
            free(tables[i].cells[r]);
        }
        free(tables[i].cells);
        free(tables[i].name);
    }
    free(tables);
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
        if (*ptr) { *ptr = '\0'; ptr++; }

        if (strcmp(cmd, "TABLE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* r_str = ptr;
            while (*ptr && !isspace((unsigned char)*ptr)) ptr++;
            if (*ptr) { *ptr = '\0'; ptr++; }
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* c_str = ptr;

            int rows = atoi(r_str);
            int cols = atoi(c_str);
            if (rows <= 0 || cols <= 0) continue;

            char*** cells = malloc(rows * sizeof(char**));
            if (!cells) { cleanup(); free(line); return 1; }
            for (int r = 0; r < rows; r++) {
                cells[r] = malloc(cols * sizeof(char*));
                if (!cells[r]) { 
                    for(int k=0; k<r; k++) free(cells[k]);
                    free(cells); cleanup(); free(line); return 1; 
                }
                for (int c = 0; c < cols; c++) cells[r][c] = NULL;
            }

            if (t_count == t_cap) {
                size_t new_cap = t_cap == 0 ? 4 : t_cap * 2;
                Table* temp = realloc(tables, new_cap * sizeof(Table));
                if (!temp) { 
                    for (int r = 0; r < rows; r++) free(cells[r]);
                    free(cells); cleanup(); free(line); return 1; 
                }
                tables = temp; t_cap = new_cap;
            }
            tables[t_count].name = strdup(name);
            tables[t_count].rows = rows;
            tables[t_count].cols = cols;
            tables[t_count].cells = cells;
            if (!tables[t_count].name) {
                for (int r = 0; r < rows; r++) free(cells[r]);
                free(cells); cleanup(); free(line); return 1;
            }
            t_count++;
        } else if (strcmp(cmd, "SET") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* r_str = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* c_str = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* val = ptr;

            int r = atoi(r_str);
            int c = atoi(c_str);

            for (size_t i = 0; i < t_count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                        free(tables[i].cells[r][c]);
                        tables[i].cells[r][c] = strdup(val);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* name = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* r_str = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* c_str = ptr;

            int r = atoi(r_str);
            int c = atoi(c_str);

            for (size_t i = 0; i < t_count; i++) {
                if (strcmp(tables[i].name, name) == 0) {
                    if (r >= 0 && r < tables[i].rows && c >= 0 && c < tables[i].cols) {
                        printf("%s\n", tables[i].cells[r][c] ? tables[i].cells[r][c] : "EMPTY");
                    } else {
                        printf("EMPTY\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < t_count; i++) {
                if (strcmp(tables[i].name, ptr) == 0) {
                    for (int r = 0; r < tables[i].rows; r++) {
                        for (int c = 0; c < tables[i].cols; c++) free(tables[i].cells[r][c]);
                        free(tables[i].cells[r]);
                    }
                    free(tables[i].cells);
                    free(tables[i].name);
                    for (size_t j = i; j < t_count - 1; j++) tables[j] = tables[j + 1];
                    t_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < t_count; i++) {
                printf("%s\n", tables[i].name);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}