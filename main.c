#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <stdbool.h>

#define MAX_SENTENCE_LEN 20*100

typedef struct Node {
    struct MarkovNode *data;
    struct Node *next;
} Node;

typedef struct LinkedList {
    Node *first;
    Node *last;
    int size;
} LinkedList;

typedef struct NextNodeCounter {
    struct MarkovNode *markov_node;
    int frequency;
} NextNodeCounter;

typedef struct MarkovNode {
    char *data;
    NextNodeCounter **counter_list;
    int next_word_num;
} MarkovNode;

typedef struct MarkovChain {
    LinkedList *database;
} MarkovChain;

int get_random_number(int max_number) {
    return rand() % max_number;
}

// given random number the function returns the word in this random place.

MarkovNode *get_first_random_node(MarkovChain *markov_chain) {
    bool keep_randomize = true;
    while (keep_randomize) {
        int random_node_num =
                get_random_number(markov_chain->database->size);
        MarkovNode *result = markov_chain->database->first->data;
        if (random_node_num > 0) {
            Node *tmp = markov_chain->database->first;
            while (random_node_num > 0) {
                tmp = tmp->next;
                random_node_num--;
            }
            result = tmp->data;
        }
        if (result->data[strlen(result->data) - 1] != '.') {
            return result;
        }
    }
}

int number_of_nexts(MarkovNode *state_struct_ptr) {
    int result = 0;
    if (state_struct_ptr->counter_list != NULL) {
        for (int i = 0; i < state_struct_ptr->next_word_num; i++) {
            result += state_struct_ptr->counter_list[i]->frequency;
        }
        return result;
    }
}

//given a pointer to a node we choose the next word depend on
//its occurrence frequency.
MarkovNode *get_next_random_node(MarkovNode *state_struct_ptr) {
    int max_number = number_of_nexts(state_struct_ptr);
    if (max_number == 0) {
        return NULL;
    }
    int random_next_word = get_random_number(max_number);
    int sum_until = 0;
    for (int i = 0; state_struct_ptr->next_word_num > i; i++) {
        sum_until += state_struct_ptr->counter_list[i]->frequency;
        if (sum_until > random_next_word) {
            return state_struct_ptr->counter_list[i]->markov_node;
        }
    }
}

void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length) {
    if (first_node == NULL) {
        first_node = get_first_random_node(markov_chain);
    }
    char result[MAX_SENTENCE_LEN] = {0};
    strcat(result, first_node->data);
    strcat(result, " ");

    while (max_length > 1) {
        MarkovNode *tmp = get_next_random_node(first_node);
        if (tmp == NULL) {
            break;
        }
        first_node = tmp;

        strcat(result, tmp->data);
        strcat(result, " ");
        max_length--;
        if (tmp->data[strlen(tmp->data) - 1] == '.') {
            break;
        }
    }
    printf("%s", result);
}

void free_markov_chain(MarkovChain **ptr_chain) {
    for (int i = 0; i < ptr_chain[0]->database->size; i++)
    {
        Node *save_ptr_normal_node = ptr_chain[0]->database->first->next;
        printf("%d",ptr_chain[0]->database->first->data->next_word_num);
        for (int x=0; x<ptr_chain[0]->database->first->data->next_word_num;x++)
        {
            free(ptr_chain[0]->database->first->data->counter_list[x]);
        }
        free(ptr_chain[0]->database->first->data->counter_list);
        free(ptr_chain[0]->database->first->data);
        free(ptr_chain[0]->database->first);
        ptr_chain[0]->database->first = save_ptr_normal_node;

    }
    free(*ptr_chain);
}

bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode *second_node) {
    if (first_node->counter_list != NULL) {
        for (int i = 0; i < first_node->next_word_num; i++) {
            if (second_node == first_node->counter_list[i]->markov_node) {
                first_node->counter_list[i]->frequency++;
                return true;
            }
        }
    }
    first_node->counter_list = (NextNodeCounter **) realloc(first_node->counter_list,
                                                            (first_node->next_word_num + 1) * sizeof
                                                                    (void *));
    if (first_node->counter_list == NULL) {
        return false;
    }
    first_node->next_word_num++;
    first_node->counter_list[first_node->next_word_num - 1] =
            (NextNodeCounter *) malloc(sizeof(NextNodeCounter));
    if (first_node->counter_list == NULL) {
        return false;
    }
    first_node->counter_list[first_node->next_word_num - 1]->markov_node
            = second_node;
    first_node->counter_list[first_node->next_word_num - 1]->frequency = 1;
    return true;
}

int main() {
    LinkedList L;
    L.size = 2;

    MarkovNode *A;
    A = (MarkovNode *) calloc(1, sizeof(MarkovNode));
    MarkovNode *B;
    B = (MarkovNode *) calloc(1, sizeof(MarkovNode));
    A->next_word_num = 1;
    B->next_word_num = 1;
    A->data = ".aaa.";
    B->data = ".bbb";
    A->counter_list = (NextNodeCounter **) malloc(sizeof(void *));
    B->counter_list = (NextNodeCounter **) malloc(sizeof(void *));

    A->counter_list[0] = (NextNodeCounter *) malloc(sizeof(NextNodeCounter));
    A->counter_list[0]->markov_node = B;
    A->counter_list[0]->frequency = 1;
    B->counter_list[0] = (NextNodeCounter *) malloc(sizeof(NextNodeCounter));
    B->counter_list[0]->markov_node = A;
    B->counter_list[0]->frequency = 1;

    Node *a_node;
    a_node = (Node *) malloc(sizeof(Node));
    Node *b_node;
    b_node = (Node *) malloc(sizeof(Node));

    a_node->data = A;
    b_node->data = B;
    a_node->next = b_node;
    b_node->next = NULL;
    MarkovChain *MAIN;
    MAIN = (MarkovChain *) calloc(1, sizeof(MarkovChain));
    MAIN->database = &L;
    L.first = a_node;
    L.last = b_node;

     //first function try
    MarkovNode *random_first_node = get_first_random_node(MAIN);
     //second function try
    MarkovNode *next_random = get_next_random_node(random_first_node);
     //third function try
    generate_random_sequence(MAIN, random_first_node, 3);
    //Fifth function try
    //add_node_to_counter_list (B, B);
     //forth

    free_markov_chain(&MAIN);
}
