/* F001.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *id;
    char *text;
} Paragraph;

static Paragraph *paragraphs = NULL;
static size_t pcount = 0, pcap = 0;

static void add_paragraph(const char *id, const char *text) {
    if (pcount == pcap) {
        pcap = pcap ? pcap * 2 : 4;
        paragraphs = realloc(paragraphs, pcap * sizeof(Paragraph));
        if (!paragraphs) { perror("realloc"); exit(1); }
    }
    paragraphs[pcount].id = strdup(id);
    paragraphs[pcount].text = strdup(text);
    pcount++;
}

static Paragraph *find_paragraph(const char *id) {
    for (size_t i = 0; i < pcount; i++) {
        if (strcmp(paragraphs[i].id, id) == 0)
            return &paragraphs[i];
    }
    return NULL;
}

static void remove_paragraph(const char *id) {
    for (size_t i = 0; i < pcount; i++) {
        if (strcmp(paragraphs[i].id, id) == 0) {
            free(paragraphs[i].id);
            free(paragraphs[i].text);
            for (size_t j = i; j < pcount - 1; j++) {
                paragraphs[j] = paragraphs[j + 1];
            }
            pcount--;
            return;
        }
    }
}

static void show_paragraph(const char *id) {
    Paragraph *p = find_paragraph(id);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *c = p->text; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

static void list_paragraphs(void) {
    for (size_t i = 0; i < pcount; i++) {
        printf("%s\n", paragraphs[i].id);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            add_paragraph(id, text);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            Paragraph *p = find_paragraph(id);
            if (p) {
                size_t newlen = strlen(p->text) + strlen(text) + 1;
                char *newtext = malloc(newlen);
                if (!newtext) { perror("malloc"); exit(1); }
                sprintf(newtext, "%s%s", p->text, text);
                free(p->text);
                p->text = newtext;
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *id = strtok(NULL, " ");
            if (!id) continue;
            char *text = strtok(NULL, "");
            if (!text) text = "";
            Paragraph *p = find_paragraph(id);
            if (p) {
                free(p->text);
                p->text = strdup(text);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = strtok(NULL, " ");
            if (id) remove_paragraph(id);
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *id = strtok(NULL, " ");
            if (id) show_paragraph(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paragraphs();
        }
    }
    for (size_t i = 0; i < pcount; i++) {
        free(paragraphs[i].id);
        free(paragraphs[i].text);
    }
    free(paragraphs);
    return 0;
}