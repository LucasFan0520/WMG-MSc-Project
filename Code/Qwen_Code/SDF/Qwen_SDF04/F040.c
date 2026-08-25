// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char* id; char* header; char* payload; int h_len, p_len; } Packet;

Packet* packets = NULL;
size_t p_count = 0, p_cap = 0;

void cleanup() {
    for (size_t i = 0; i < p_count; i++) { free(packets[i].id); free(packets[i].header); free(packets[i].payload); }
    free(packets);
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

        if (strcmp(cmd, "PACKET") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* id = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* hl_str = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* pl_str = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* header = ptr; while (*ptr && !isspace((unsigned char)*ptr)) ptr++; if (*ptr) {*ptr='\0'; ptr++;}
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            char* payload = ptr;

            int hl = atoi(hl_str);
            int pl = atoi(pl_str);
            if ((int)strlen(header) != hl || (int)strlen(payload) != pl) continue;

            if (p_count == p_cap) {
                size_t new_cap = p_cap == 0 ? 4 : p_cap * 2;
                Packet* temp = realloc(packets, new_cap * sizeof(Packet));
                if (!temp) { cleanup(); free(line); return 1; }
                packets = temp; p_cap = new_cap;
            }
            packets[p_count].id = strdup(id);
            packets[p_count].header = strdup(header);
            packets[p_count].payload = strdup(payload);
            packets[p_count].h_len = hl;
            packets[p_count].p_len = pl;
            if (!packets[p_count].id || !packets[p_count].header || !packets[p_count].payload) {
                cleanup(); free(line); return 1;
            }
            p_count++;
        } else if (strcmp(cmd, "DROP") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(packets[i].id, ptr) == 0) {
                    free(packets[i].id); free(packets[i].header); free(packets[i].payload);
                    for (size_t j = i; j < p_count - 1; j++) packets[j] = packets[j + 1];
                    p_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "HEADER") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(packets[i].id, ptr) == 0) {
                    printf("%s\n", packets[i].header);
                    break;
                }
            }
        } else if (strcmp(cmd, "PAYLOAD") == 0) {
            while (*ptr && isspace((unsigned char)*ptr)) ptr++;
            for (size_t i = 0; i < p_count; i++) {
                if (strcmp(packets[i].id, ptr) == 0) {
                    printf("%s\n", packets[i].payload);
                    break;
                }
            }
        } else if (strcmp(cmd, "STATS") == 0) {
            long total = 0;
            for (size_t i = 0; i < p_count; i++) total += packets[i].p_len;
            printf("%ld\n", total);
        }
    }
    free(line);
    cleanup();
    return 0;
}