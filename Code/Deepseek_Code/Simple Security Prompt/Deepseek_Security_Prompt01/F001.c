// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; char *text; } Para;
Para *paras = NULL;
int para_count = 0, para_cap = 0;

void add_para(const char *id, const char *text) {
    for (int i = 0; i < para_count; i++)
        if (strcmp(paras[i].id, id) == 0) return;
    if (para_count == para_cap) {
        para_cap = para_cap ? para_cap * 2 : 4;
        paras = realloc(paras, sizeof(Para) * para_cap);
    }
    paras[para_count].id = strdup(id);
    paras[para_count].text = strdup(text);
    para_count++;
}

void append_para(const char *id, const char *text) {
    for (int i = 0; i < para_count; i++) {
        if (strcmp(paras[i].id, id) == 0) {
            char *newtext = malloc(strlen(paras[i].text) + strlen(text) + 1);
            strcpy(newtext, paras[i].text);
            strcat(newtext, text);
            free(paras[i].text);
            paras[i].text = newtext;
            return;
        }
    }
}

void replace_para(const char *id, const char *text) {
    for (int i = 0; i < para_count; i++) {
        if (strcmp(paras[i].id, id) == 0) {
            free(paras[i].text);
            paras[i].text = strdup(text);
            return;
        }
    }
}

void delete_para(const char *id) {
    for (int i = 0; i < para_count; i++) {
        if (strcmp(paras[i].id, id) == 0) {
            free(paras[i].id);
            free(paras[i].text);
            for (int j = i; j < para_count - 1; j++) paras[j] = paras[j + 1];
            para_count--;
            return;
        }
    }
}

void show_para(const char *id) {
    for (int i = 0; i < para_count; i++) {
        if (strcmp(paras[i].id, id) == 0) {
            for (char *p = paras[i].text; *p; p++) putchar(*p == ' ' ? '_' : *p);
            putchar('\n');
            return;
        }
    }
    printf("NOT_FOUND\n");
}

void list_paras(void) {
    for (int i = 0; i < para_count; i++) printf("%s\n", paras[i].id);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], id[256], rest[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "NEW") == 0 || strcmp(cmd, "APPEND") == 0 || strcmp(cmd, "REPLACE") == 0) {
            char *p = line + strlen(cmd);
            while (*p == ' ') p++;
            char *id_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            strcpy(id, id_start);
            strcpy(rest, p);
            if (strcmp(cmd, "NEW") == 0) add_para(id, rest);
            else if (strcmp(cmd, "APPEND") == 0) append_para(id, rest);
            else replace_para(id, rest);
        } else if (strcmp(cmd, "DELETE") == 0 || strcmp(cmd, "SHOW") == 0) {
            char *p = line + strlen(cmd);
            while (*p == ' ') p++;
            strcpy(id, p);
            if (strcmp(cmd, "DELETE") == 0) delete_para(id);
            else show_para(id);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_paras();
        }
    }
    for (int i = 0; i < para_count; i++) { free(paras[i].id); free(paras[i].text); }
    free(paras);
    return 0;
}