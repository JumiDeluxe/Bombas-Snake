#ifndef SCORES_H
#define SCORES_H

typedef struct score_entry {
    char player_name[32];
    int value;
    struct score_entry* next;
}score_entry;

score_entry* create_score_entry(score_entry* scores, char* name, int score);
score_entry* read_scores();
void print_scores();
void save_scores(score_entry* scores);
void position_score_in_ranking(score_entry** head_ref, score_entry* new_node);
void add_new_score(int score);

#endif