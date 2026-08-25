// F002.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *message;
} Record;

int main(void) {
    Record *records = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *tag_start = line + 4;
            while (*tag_start == ' ') tag_start++;
            char *space = strchr(tag_start, ' ');
            char *msg_start = "";
            if (space) {
                *space = '\0';
                msg_start = space + 1;
                while (*msg_start == ' ') msg_start++;
            }
            if (count >= capacity) {
                int new_cap = capacity == 0 ? 4 : capacity * 2;
                Record *tmp = realloc(records, new_cap * sizeof(Record));
                if (!tmp) {
                    free(line);
                    for(int i=0; i<count; i++) { free(records[i].tag); free(records[i].message); }
                    free(records);
                    return 1;
                }
                records = tmp;
                capacity = new_cap;
            }
            char *t = strdup(tag_start);
            char *m = strdup(msg_start);
            if (!t || !m) {
                free(t); free(m); free(line);
                for(int i=0; i<count; i++) { free(records[i].tag); free(records[i].message); }
                free(records);
                return 1;
            }
            records[count].tag = t;
            records[count].message = m;
            count++;
        } else if (strncmp(line, "CONCAT ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *tag1 = p;
            char *space1 = strchr(tag1, ' ');
            if (!space1) continue;
            *space1 = '\0';
            char *tag2 = space1 + 1;
            while (*tag2 == ' ') tag2++;
            char *space2 = strchr(tag2, ' ');
            if (!space2) continue;
            *space2 = '\0';
            char *newtag = space2 + 1;
            while (*newtag == ' ') newtag++;
            
            int f1 = -1, f2 = -1;
            for (int i = 0; i < count; i++) {
                if (f1 == -1 && strcmp(records[i].tag, tag1) == 0) f1 = i;
                if (f2 == -1 && strcmp(records[i].tag, tag2) == 0) f2 = i;
            }
            if (f1 != -1 && f2 != -1) {
                size_t l1 = strlen(records[f1].message);
                size_t l2 = strlen(records[f2].message);
                char *nmsg = malloc(l1 + l2 + 2);
                if (!nmsg) {
                    free(line);
                    for(int i=0; i<count; i++) { free(records[i].tag); free(records[i].message); }
                    free(records);
                    return 1;
                }
                strcpy(nmsg, records[f1].message);
                strcat(nmsg, " ");
                strcat(nmsg, records[f2].message);
                
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Record *tmp = realloc(records, new_cap * sizeof(Record));
                    if (!tmp) {
                        free(nmsg); free(line);
                        for(int i=0; i<count; i++) { free(records[i].tag); free(records[i].message); }
                        free(records);
                        return 1;
                    }
                    records = tmp;
                    capacity = new_cap;
                }
                char *nt = strdup(newtag);
                if (!nt) {
                    free(nmsg); free(line);
                    for(int i=0; i<count; i++) { free(records[i].tag); free(records[i].message); }
                    free(records);
                    return 1;
                }
                records[count].tag = nt;
                records[count].message = nmsg;
                count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *tag_start = line + 7;
            while (*tag_start == ' ') tag_start++;
            int w = 0;
            for (int r = 0; r < count; r++) {
                if (strcmp(records[r].tag, tag_start) == 0) {
                    free(records[r].tag);
                    free(records[r].message);
                } else {
                    records[w++] = records[r];
                }
            }
            count = w;
        } else if (strncmp(line, "FIND ", 5) == 0) {
            char *tag_start = line + 5;
            while (*tag_start == ' ') tag_start++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(records[i].tag, tag_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *m = records[found].message;
                while (*m) {
                    if (*m == ' ') putchar('_');
                    else putchar(*m);
                    m++;
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strcmp(line, "REPORT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", records[i].tag, records[i].message);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(records[i].tag);
        free(records[i].message);
    }
    free(records);
    return 0;
}
