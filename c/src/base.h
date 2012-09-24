#ifndef BASE_HEADER
#define BASE_HEADER

struct base_struct
{
  char value;
  char terminal;
};

typedef struct base_struct base;

void base_init(base *);

#endif
