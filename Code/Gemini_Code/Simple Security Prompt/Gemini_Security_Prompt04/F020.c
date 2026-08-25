// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *title;
    char **body;
    int bcount;
    int bcap;
} Catalog;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *fp = fopen(argv[1], "r");
    if (!fp) return 1;
    Catalog *cats = NULL;
    int count = 0;
    int cap = 0;
    while (1) {
        char *title = read_line(fp);
        if (!title) break;
        if (strcmp(title, "--") == 0) {
            free(title);
            continue;
        }
        if (count >= cap) {
            cap = cap == 0 ? 16 : cap * 2;
            cats = realloc(cats, cap * sizeof(Catalog));
        }
        cats[count].title = title;
        cats[count].body = NULL;
        cats[count].bcount = 0;
        cats[count].bcap = 0;
        while (1) {
            char *bline = read_line(fp);
            if (!bline) break;
            if (strcmp(bline, "--") == 0) {
                free(bline);
                break;
            }
            Catalog *c = &cats[count];
            if (c->bcount >= c->bcap) {
                c->bcap = c->bcap == 0 ? 16 : c->bcap * 2;
                c->body = realloc(c->body, c->bcap * sizeof(char *));
            }
            c->body[c->bcount] = bline;
            c->bcount++;
        }
        count++;
    }
    fclose(fp);
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "FIND ", 5) == 0) {
            char *title = line + 5;
            while (*title == ' ') title++;
            for (int i = 0; i < count; i++) {
                if (strcmp(cats[i].title, title) == 0) {
                    for (int j = 0; j < cats[i].bcount; j++) {
                        printf("%s\n", cats[i].body[j]);
                    }
                    break;
                }
            }
        } else if (strncmp(line, "DELETE ", 7) == 0) {
            char *title = line + 7;
            while (*title == ' ') title++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(cats[i].title, title) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(cats[found].title);
                for (int j = 0; j < cats[found].bcount; j++) free(cats[found].body[j]);
                free(cats[found].body);
                for (int i = found; i < count - 1; i++) {
                    cats[i] = cats[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "ADD ", 4) == 0) {
            char *title = line + 4;
            while (*title == ' ') title++;
            if (count >= cap) {
                cap = cap == 0 ? 16 : cap * 2;
                cats = realloc(cats, cap * sizeof(Catalog));
            }
            cats[count].title = strdup(title);
            cats[count].body = NULL;
            cats[count].bcount = 0;
            cats[count].bcap = 0;
            while (1) {
                char *bline = read_line(stdin);
                if (!bline) break;
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                Catalog *c = &cats[count];
                if (c->bcount >= c->bcap) {
                    c->bcap = c->bcap == 0 ? 16 : c->bcap * 2;
                    c->body = realloc(c->body, c->bcap * sizeof(char *));
                }
                c->body[c->bcount] = bline;
                c->bcount++;
            }
            count++;
        } else if (strcmp(line, "PRINT") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s\n", cats[i].title);
                for (int j = 0; j < cats[i].bcount; j++) {
                    printf("%s\n", cats[i].body[j]);
                }
                if (i < count - 1) printf("--\n");
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(cats[i].title);
        for (int j = 0; j < cats[i].bcount; j++) free(cats[i].body[j]);
        free(cats[i].body);
    }
    free(cats);
    return 0;
}
