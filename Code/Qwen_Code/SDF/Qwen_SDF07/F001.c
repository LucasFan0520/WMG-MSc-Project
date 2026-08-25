// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *id; char *text; } Para;
Para *p = NULL;
size_t pc = 0, pcap = 0;

void add_p(const char *id, const char *text) {
    for (size_t i = 0; i < pc; i++) {
        if (strcmp(p[i].id, id) == 0) {
            char *tmp = strdup(text);
            if (!tmp) { perror("strdup"); exit(1); }
            free(p[i].text);
            p[i].text = tmp;
            return;
        }
    }
    if (pc == pcap) {
        size_t nc = pcap ? pcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Para)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Para *tmp = realloc(p, nc * sizeof(Para));
        if (!tmp) { perror("realloc"); exit(1); }
        p = tmp;
        pcap = nc;
    }
    p[pc].id = strdup(id);
    p[pc].text = strdup(text);
    if (!p[pc].id || !p[pc].text) { perror("strdup"); exit(1); }
    pc++;
}

void show_p(const char *id) {
    for (size_t i = 0; i < pc; i++) {
        if (strcmp(p[i].id, id) == 0) {
            for (size_t j = 0; p[i].text[j]; j++) putchar(p[i].text[j] == ' ' ? '_' : p[i].text[j]);
            putchar('\n');
            return;
        }
    }
    printf("NOT_FOUND\n");
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) != 1) { free(line); continue; }
            char *text = strchr(line, ' ');
            if (text) { text = strchr(text + 1, ' '); if (text) text++; }
            if (!text) text = "";
            if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "REPLACE") == 0) add_p(id, text);
            else {
                for (size_t i = 0; i < pc; i++) {
                    if (strcmp(p[i].id, id) == 0) {
                        size_t l1 = strlen(p[i].text), l2 = strlen(text);
                        if (l1 + l2 + 2 < l1) { fprintf(stderr, "Overflow\n"); exit(1); }
                        char *tmp = malloc(l1 + l2 + 2);
                        if (!tmp) { perror("malloc"); exit(1); }
                        snprintf(tmp, l1 + l2 + 2, "%s %s", p[i].text, text);
                        free(p[i].text);
                        p[i].text = tmp;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) {
                for (size_t i = 0; i < pc; i++) {
                    if (strcmp(p[i].id, id) == 0) {
                        free(p[i].id); free(p[i].text);
                        p[i] = p[--pc];
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) show_p(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < pc; i++) printf("%s\n", p[i].id);
        }
        free(line);
    }
    for (size_t i = 0; i < pc; i++) { free(p[i].id); free(p[i].text); }
    free(p);
    return 0;
}