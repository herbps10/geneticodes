#include "base_tree.h"

base_tree *base_tree_create(base *base)
{
  base_tree *tree = (base_tree *) malloc(sizeof(base_tree));

  base_node *root = base_node_create();
  root->base = base;

  tree->root = root;
}

double base_tree_eval_iter(chromosome *chromosome, base_node *node, double S, double I, double R)
{
  base_node *stack[EQN_GENE_LENGTH];
  int stack_top = 0;

  base_node *current = node;

  double value = 0;
  char op_state = ' ';

  do {
    while(current != NULL)
    {
      stack_top++;
      stack[stack_top] = current;
      current = current->prev;
    }

    if(stack_top > 0)
    {
      current = stack[stack_top];
      stack_top--;
      printf("%c\n", current->base->value);

      if(current->base->terminal == true && op_state == ' ')
      {
        value = 2.0;
      }
      else if(current->base->terminal == true && op_state != ' ')
      {
        if(op_state == '*')
        {
          value = value * 2.0;
        }
        else if(op_state == '+')
        {
          value = value + 2.0;
        }
        else if(op_state == '-')
        {
          value = value - 2.0;
        }
      }
      else if(current->base->terminal == false)
      {
        op_state = current->base->value;
      }

      current = current->next;
    }
  } while(stack_top > 0 || current != NULL);

  return value;
}

double base_tree_eval(chromosome *chromosome, base_node *node, double S, double I, double R)
{
  if(node->base->terminal == true)
  {
    if(node->base->value == '1')
    {
      return -1;
    } 
    else if(node->base->value == '!')
    {
      return 1;
    }
    else if(node->base->value == '0')
    {
      return 0;
    }
    else if(node->base->value == 'm')
    {
      return chromosome->mu;
    }
    else if(node->base->value == 'b')
    {
      return chromosome->beta;
    }
    else if(node->base->value == 'g')
    {
      return chromosome->gamma;
    }
    else if(node->base->value == 'S')
    {
      return S;
    }
    else if(node->base->value == 'I')
    {
      return I;
    }
    else if(node->base->value == 'R')
    {
      return R;
    }
  }

  if(node->base->value == '+')
  {
     return base_tree_eval(chromosome, node->prev, S, I, R) + base_tree_eval(chromosome, node->next, S, I, R);
  }
  else if(node->base->value == '-')
  {
     return base_tree_eval(chromosome, node->prev, S, I, R) - base_tree_eval(chromosome, node->next, S, I, R);
  }
  else if(node->base->value == '*')
  {
     return base_tree_eval(chromosome, node->prev, S, I, R) * base_tree_eval(chromosome, node->next, S, I, R);
  }
}


void base_tree_display(base_node *node)
{
  if(node->base->terminal == true)
  {
    if(node->base->value == '1')
    {
      printf("-1");
    }
    else if(node->base->value == '!')
    {
      printf("1");
    } 
    else
    {
      printf("%c", node->base->value);
    }
  }
  else
  {
    printf("(");

    if(node->prev != NULL)
    {
      base_tree_display(node->prev);
    }

    printf("%c", node->base->value);

    if(node->next != NULL)
    {
      base_tree_display(node->next);
    }

    printf(")");
  }
}

char *base_tree_string(base_node *node)
{
  char *str;

  if(node->base->terminal == true)
  {
    if(node->base->value == '1')
    {
      str = "-1";
    }
    else if(node->base->value == '!')
    {
      str = "1";
    } 
    else
    {
      str = malloc(sizeof(char) * 2);
      sprintf(str, "%c", node->base->value);
    }
  }
  else
  {
    char *left, *right;

    left = base_tree_string(node->prev);

    right = base_tree_string(node->next);

    str = malloc(sizeof(char) * (4 + strlen(left) + strlen(right)));
    sprintf(str, "%c%s%c%s%c", '(', left, node->base->value, right, ')');
  }

  return str;
}

void base_tree_destroy(base_node *node)
{
  if(node->prev != NULL)
  {
    base_tree_destroy(node->prev);
  }
  
  if(node->next != NULL)
  {
    base_tree_destroy(node->next);
  }

  free(node);
}
