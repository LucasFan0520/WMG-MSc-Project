// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *tag;
    char *msg;
} Record;

Record *recs = NULL;
int rec_count = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char tag[100], msg[1000];
            scanf("%s", tag);
            scanf(" %[^\n]", msg);
            recs = realloc(recs, sizeof(Record) * (rec_count + 1));
            recs[rec_count].tag = strdup(tag);
            recs[rec_count].msg = strdup(msg);
            rec_count++;
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char t1[100], t2[100], nt[100];
            scanf("%s %s %s", t1, t2, nt);
            char *m1 = NULL, *m2 = NULL;
            for (int i = 0; i < rec_count; i++) {
                if (!m1 && strcmp(recs[i].tag, t1) == 0) m1 = recs[i].msg;
                if (!m2 && strcmp(recs[i].tag, t2) == 0) m2 = recs[i].msg;
            }
            if (m1 && m2) {
                char *new_msg = malloc(strlen(m1) + strlen(m2) + 2);
                sprintf(new_msg, "%s %s", m1, m2);
                recs = realloc(recs, sizeof(Record) * (rec_count + 1));
                recs[rec_count].tag = strdup(nt);
                recs[rec_count].msg = new_msg;
                rec_count++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char tag[100];
            scanf("%s", tag);
            for (int i = 0; i < rec_count; ) {
                if (strcmp(recs[i].tag, tag) == 0) {
                    free(recs[i].tag);
                    free(recs[i].msg);
                    for (int j = i; j < rec_count - 1; j++) recs[j] = recs[j+1];
                    rec_count--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char tag[100];
            scanf("%s", tag);
            for (int i = 0; i < rec_count; i++) {
                if (strcmp(recs[i].tag, tag) == 0) {
                    for (char *p = recs[i].msg; *p; p++) {
                        if (*p == ' ') putchar('_');
                        else putchar(*p);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rec_count; i++) {
                printf("%s %s\n", recs[i].tag, recs[i].msg);
            }
        }
    }
    for (int i = 0; i < rec_count; i++) {
        free(recs[i].tag);
        free(recs[i].msg);
    }
    free(recs);
    return 0;
}