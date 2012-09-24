#include <stdio.h>
#include <stdlib.h>

#define OPERATOR 1
#define TERMINAL 2

struct Node {
  struct Node *right;
  struct Node *left;
  int type;

  char operator;
  char terminal;
};

typedef struct Node node;

char eval_expr(node *node) {
  if(node->type == TERMINAL) {
    return node->terminal;
  }
  else {
    if(node->operator == '+') {
      return eval_expr(node->right) + eval_expr(node->left);
    }
  }
}

node build_tree(char *input) {
  char c;
  int index = 0;
  node *start = NULL;
  node **row = malloc(sizeof(node *) * 3);
  
  int current_branch_index = 0;

  while(input[index] != '\0') {

    if(input[index] == '+') {
      printf("%c\n", input[index]);

      if(start == NULL) {
        start = malloc(sizeof(node));
        start->type = OPERATOR;
        start->operator = '+';

        row[0] = start;
        current_branch_index = 0;
      }
      else {
      }
    }
    else {

    }

    index++;
  }
}

int main() {
  char *K = "+23";

  build_tree(K);

  return 0;

  node *n1 = malloc(sizeof(node));
  node *n2 = malloc(sizeof(node));
  node *n3 = malloc(sizeof(node));

  n1->type = OPERATOR;
  n1->operator = '+';

  n2->type = TERMINAL;
  n2->terminal = 1;

  n3->type = TERMINAL;
  n3->terminal = 5;

  n1->right = n2;
  n1->left = n3;

  char result = eval_expr(n1);

  printf("%i\n", result);

  return 0;
}
