// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

char* get_word(char** str) {
    while (**str == ' ') (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && **str != ' ') (*str)++;
    if (**str) {
        **str = '\0';
        (*str)++;
    }
    return start;
}

typedef struct Record {
    char* title;
    char* body;
    struct Record* next;
} Record;

Record* head = NULL;

Record* find_record(const char* title) {
    for (Record* r = head; r; r = r->next) {
        if (strcmp(r->title, title) == 0) return r;
    }
    return NULL;
}

void add_record(const char* title, const char* body) {
    Record* n = malloc(sizeof(Record));
    n->title = strdup(title);
    n->body = strdup(body);
    n->next = head;
    head = n;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        FILE* f = fopen(argv[1], "r");
        if (f) {
            char l[MAX_LINE];
            char title[MAX_LINE] = {0};
            char body[MAX_LINE * 10] = {0};
            int in_record = 0;
            
            while (fgets(l, sizeof(l), f)) {
                l[strcspn(l, "\n")] = 0;
                if (strcmp(l, "--") == 0) {
                    if (in_record) {
                        add_record(title, body);
                        in_record = 0;
                        title[0] = 0;
                        body[0] = 0;
                    }
                } else {
                    if (!in_record) {
                        strcpy(title, l);
                        in_record = 1;
                    } else {
                        if (strlen(body) > 0) strcat(body, "\n");
                        strcat(body, l);
                    }
                }
            }
            if (in_record) add_record(title, body);
            fclose(f);
        }
    }
    
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;
        
        if (strcmp(cmd, "FIND") == 0) {
            char* title = get_word(&p);
            Record* r = find_record(title);
            if (r) {
                printf("%s\n%s\n", r->title, r->body);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* title = get_word(&p);
            Record* prev = NULL;
            for (Record* r = head; r; prev = r, r = r->next) {
                if (strcmp(r->title, title) == 0) {
                    if (prev) prev->next = r->next;
                    else head = r->next;
                    free(r->title);
                    free(r->body);
                    free(r);
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char* title = get_word(&p);
            char body[MAX_LINE * 10] = {0};
            char bline[MAX_LINE];
            while (fgets(bline, sizeof(bline), stdin)) {
                bline[strcspn(bline, "\n")] = 0;
                if (strcmp(bline, "--") == 0) break;
                if (strlen(body) > 0) strcat(body, "\n");
                strcat(body, bline);
            }
            add_record(title, body);
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (Record* r = head; r; r = r->next) {
                printf("%s\n%s\n--\n", r->title, r->body);
            }
        }
    }
    return 0;
}