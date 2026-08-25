// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* name; char* note; } Record;

Record* records = NULL;
size_t r_count = 0, r_cap = 0;

void cleanup() {
    for (size_t i = 0; i < r_count; i++) {
        free(records[i].name);
        free(records[i].note);
    }
    free(records);
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

        if (strcmp(cmd, "BATCH") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            int n = atoi(ptr);
            if (n <= 0) continue;

            Record* temp_recs = malloc(n * sizeof(Record));
            if (!temp_recs) { cleanup(); free(line); return 1; }
            int valid = 1;
            for (int i = 0; i < n; i++) {
                if (getline(&line, &len, stdin) == -1) { valid = 0; break; }
                if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
                
                char* p = line;
                while (*p && isspace((unsigned char)*p)) p++;
                char* name = p;
                while (*p && !isspace((unsigned char)*p)) p++;
                if (*p) { *p = '\0'; p++; }
                while (*p && isspace((unsigned char)*p)) p++;
                char* note = p;
                
                if (!*name) { valid = 0; break; }
                temp_recs[i].name = strdup(name);
                temp_recs[i].note = strdup(note);
                if (!temp_recs[i].name || !temp_recs[i].note) { valid = 0; break; }
            }

            if (valid) {
                for (int i = 0; i < n; i++) {
                    if (r_count == r_cap) {
                        size_t new_cap = r_cap == 0 ? 4 : r_cap * 2;
                        Record* temp = realloc(records, new_cap * sizeof(Record));
                        if (!temp) { 
                            for(int k=0; k<=i; k++) { free(temp_recs[k].name); free(temp_recs[k].note); }
                            for(int k=i+1; k<n; k++) { free(temp_recs[k].name); free(temp_recs[k].note); }
                            free(temp_recs); cleanup(); free(line); return 1; 
                        }
                        records = temp; r_cap = new_cap;
                    }
                    records[r_count++] = temp_recs[i];
                }
            } else {
                for (int i = 0; i < n; i++) {
                    free(temp_recs[i].name);
                    free(temp_recs[i].note);
                }
            }
            free(temp_recs);
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].name, ptr) == 0) {
                    free(records[i].name);
                    free(records[i].note);
                    for (size_t j = i; j < r_count - 1; j++) records[j] = records[j + 1];
                    r_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < r_count; i++) {
                if (strcmp(records[i].name, ptr) == 0) {
                    printf("%s %s\n", records[i].name, records[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < r_count; i++) {
                printf("%s %s\n", records[i].name, records[i].note);
            }
        }
    }
    free(line);
    cleanup();
    return 0;
}