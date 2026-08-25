// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

Paragraph *paragraphs = NULL;
int count = 0;

int find_id(const char *id) {
    for (int i = 0; i < count; i++)
        if (strcmp(paragraphs[i].id, id) == 0) return i;
    return -1;
}

void new_para(const char *id, const char *text) {
    int idx = find_id(id);
    if (idx != -1) {
        free(paragraphs[idx].text);
        paragraphs[idx].text = strdup(text);
    } else {
        paragraphs = realloc(paragraphs, sizeof(Paragraph) * (count + 1));
        paragraphs[count].id = strdup(id);
        paragraphs[count].text = strdup(text);
        count++;
    }
}

void append_para(const char *id, const char *text) {
    int idx = find_id(id);
    if (idx != -1) {
        char *new_text = malloc(strlen(paragraphs[idx].text) + strlen(text) + 2);
        strcpy(new_text, paragraphs[idx].text);
        strcat(new_text, " ");
        strcat(new_text, text);
        free(paragraphs[idx].text);
        paragraphs[idx].text = new_text;
    } else {
        new_para(id, text);
    }
}

void replace_para(const char *id, const char *text) {
    int idx = find_id(id);
    if (idx != -1) {
        free(paragraphs[idx].text);
        paragraphs[idx].text = strdup(text);
    }
}

void delete_para(const char *id) {
    int idx = find_id(id);
    if (idx != -1) {
        free(paragraphs[idx].id);
        free(paragraphs[idx].text);
        for (int i = idx; i < count - 1; i++) paragraphs[i] = paragraphs[i + 1];
        count--;
    }
}

void show_para(const char *id) {
    int idx = find_id(id);
    if (idx != -1) {
        for (int i = 0; paragraphs[idx].text[i]; i++)
            putchar(paragraphs[idx].text[i] == ' ' ? '_' : paragraphs[idx].text[i]);
        putchar('\n');
    } else {
        printf("NOT_FOUND\n");
    }
}

void list_paras() {
    for (int i = 0; i < count; i++) printf("%s\n", paragraphs[i].id);
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], id[1000];
        if (sscanf(line, "%s %s", cmd, id) < 2) {
            if (strcmp(line, "LIST") == 0) list_paras();
            continue;
        }
        char *text = line + strlen(cmd) + 1 + strlen(id);
        if (*text == ' ') text++;
        if (strcmp(cmd, "NEW") == 0) new_para(id, text);
        else if (strcmp(cmd, "APPEND") == 0) append_para(id, text);
        else if (strcmp(cmd, "REPLACE") == 0) replace_para(id, text);
        else if (strcmp(cmd, "DELETE") == 0) delete_para(id);
        else if (strcmp(cmd, "SHOW") == 0) show_para(id);
    }
    for (int i = 0; i < count; i++) {
        free(paragraphs[i].id);
        free(paragraphs[i].text);
    }
    free(paragraphs);
    return 0;
}