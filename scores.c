#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scores.h"

score_entry* create_score_entry(score_entry* scores, char* name, int score) {
    score_entry* new_entry;
    new_entry = (score_entry*) malloc(sizeof(score_entry));
    strncpy(new_entry->player_name, name, 32);
    new_entry->value = score;
    new_entry->next = scores;
    return new_entry;
}

score_entry* read_scores() {
    score_entry* scores = (score_entry*)malloc(sizeof(score_entry));
    scores = NULL;
    char name[32];
    char score;
    FILE* file = fopen("scores", "r");
    rewind(file);
    while(fscanf(file, "%hhd %s\n", &score, name) != EOF) {
        printf("%s %d\n", name, score);
        if(strlen(name) > 0) scores = create_score_entry(scores, name, score);
    }
    fclose(file);
    return scores;
}

void print_scores() {
    score_entry* scores = read_scores();
    if(scores != NULL) {
        printf("%d %s\n", scores->value, scores->player_name);
        print_scores(scores->next);
    }
    return;
}

void save_scores(score_entry* scores) {
    FILE* file = fopen("scores", "w");
    while(scores != NULL) {
        fprintf(file, "%d %s\n", scores->value, scores->player_name);
        //printf("%d %s\n", scores->value, scores->player_name);
        scores = scores->next;
    }
    fclose(file);
    return;
}

void position_score_in_ranking(score_entry** head_ref, score_entry* new_node)
{
    score_entry* current;
    if (*head_ref == NULL
        || (*head_ref)->value
               >= new_node->value) {
        new_node->next = *head_ref;
        *head_ref = new_node;
    }
    else {
        /* Locate the node before
the point of insertion */
        current = *head_ref;
        while (current->next != NULL
               && current->next->value <= new_node->value) {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

void add_new_score(int score, char name[32]) {
    score_entry* scores = NULL;
    score_entry* new_score;
    new_score = (score_entry*) malloc(sizeof(score_entry));

    //printf("Wprowadź swoje imie!\n");
    //fgets(name, 32, stdin);
    strtok(name, "\n");
    strncpy(new_score->player_name, name, sizeof new_score->player_name);
    new_score->value = score;
    new_score->next = NULL;

    scores = read_scores();
    position_score_in_ranking(&scores, new_score);
    //printf("%s %d %s\n", scores->player_name, scores->value, scores->next->player_name);
    save_scores(scores);
    return;
}
/* do testow
int main() {
    score_entry* scores = NULL;

    add_new_score(13);
    //scores = read_scores();
    print_scores();
    //print_scores(scores);

    return 0;
}
*/
