// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Packet {
    char *id;
    int headerLen;
    int payloadLen;
    char *header;
    char *payload;
    struct Packet *next;
} Packet;

Packet *head = NULL;

Packet* find_packet(const char *id) {
    Packet *p = head;
    while (p) {
        if (strcmp(p->id, id) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void free_packet(Packet *p) {
    free(p->id);
    free(p->header);
    free(p->payload);
    free(p);
}

int parse_length(const char *str, int *len) {
    if (!str || !isdigit(*str)) return 0;
    *len = atoi(str);
    return 1;
}

void handle_packet(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    if (find_packet(id)) return;
    char *hstr = strtok(NULL, " ");
    char *pstr = strtok(NULL, " ");
    if (!hstr || !pstr) return;
    int hlen, plen;
    if (!parse_length(hstr, &hlen) || !parse_length(pstr, &plen)) return;
    char *header = strtok(NULL, " ");
    if (!header) return;
    char *payload = strtok(NULL, "");
    if (!payload) payload = "";
    // Verify lengths match
    if ((int)strlen(header) != hlen || (int)strlen(payload) != plen) return;
    Packet *new = malloc(sizeof(Packet));
    new->id = strdup(id);
    new->headerLen = hlen;
    new->payloadLen = plen;
    new->header = strdup(header);
    new->payload = strdup(payload);
    new->next = NULL;
    if (!head) head = new;
    else {
        Packet *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_drop(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    Packet *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->id, id) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free_packet(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void handle_header(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->header);
}

void handle_payload(char *cmd) {
    char *id = strtok(NULL, " ");
    if (!id) return;
    Packet *p = find_packet(id);
    if (p) printf("%s\n", p->payload);
}

void handle_stats() {
    int total = 0;
    Packet *p = head;
    while (p) {
        total += p->payloadLen;
        p = p->next;
    }
    printf("%d\n", total);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "PACKET") == 0) handle_packet(cmd);
        else if (strcmp(cmd, "DROP") == 0) handle_drop(cmd);
        else if (strcmp(cmd, "HEADER") == 0) handle_header(cmd);
        else if (strcmp(cmd, "PAYLOAD") == 0) handle_payload(cmd);
        else if (strcmp(cmd, "STATS") == 0) handle_stats();
    }
    while (head) {
        Packet *tmp = head;
        head = head->next;
        free_packet(tmp);
    }
    return 0;
}