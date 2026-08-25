// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Paragraph {
    char *id;
    char *text;
    struct Paragraph *next;
} Paragraph;

Paragraph *head = NULL;

Paragraph *find_para(const char *id) {
    Paragraph *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

void add_para(const char *id, const char *text) {
    Paragraph *p = find_para(id);
    if (p) {
        free(p->text);
        p->text = strdup(text);
    } else {
        p = malloc(sizeof(Paragraph));
        p->id = strdup(id);
        p->text = strdup(text);
        p->next = NULL;
        Paragraph *tail = head;
        if (!head) {
            head = p;
        } else {
            while (tail->next) {
                tail = tail->next;
            }
            tail->next = p;
        }
    }
}

void append_para(const char *id, const char *text) {
    Paragraph *p = find_para(id);
    if (p) {
        size_t len = strlen(p->text) + strlen(text) + 2;
        char *new_text = malloc(len);
        snprintf(new_text, len, "%s %s", p->text, text);
        free(p->text);
        p->text = new_text;
    } else {
        add_para(id, text);
    }
}

void delete_para(const char *id) {
    Paragraph *p = head;
    Paragraph *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) {
                prev->next = p->next;
            } else {
                head = p->next;
            }
            free(p->id);
            free(p->text);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void show_para(const char *id) {
    Paragraph *p = find_para(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *c = p->text; *c; c++) {
        if (*c == ' ') {
            putchar('_');
        } else {
            putchar(*c);
        }
    }
    putchar('\n');
}

void list_para() {
    Paragraph *p = head;
    while (p) {
        printf("%s\n", p->id);
        p = p->next;
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char id[1000];
        char text[9000];
        text[0] = 0;
        int n = sscanf(line, "%s %s %[^\n]", cmd, id, text);
        if (n >= 2) {
            if (strcmp(cmd, "NEW") == 0) {
                add_para(id, text);
            } else if (strcmp(cmd, "APPEND") == 0) {
                append_para(id, text);
            } else if (strcmp(cmd, "REPLACE") == 0) {
                add_para(id, text);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_para(id);
            } else if (strcmp(cmd, "SHOW") == 0) {
                show_para(id);
            } else if (strcmp(cmd, "LIST") == 0) {
                list_para();
            }
        }
    }
    Paragraph *p = head;
    while (p) {
        Paragraph *next = p->next;
        free(p->id);
        free(p->text);
        free(p);
        p = next;
    }
    return 0;
}