#include "base.h"

base *base_create()
{
  base *base = malloc(sizeof(base));

  base_init(base);

  return base;
}

void base_init(base *base) {
  base->terminal = true;
  base->value = 'P';
}

void base_destroy(base *base) {
  free(base);
}
