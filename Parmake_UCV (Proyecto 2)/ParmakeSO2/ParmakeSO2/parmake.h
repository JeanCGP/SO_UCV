#ifndef PARMAKE_H
#define PARMAKE_H

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "parser.h"

enum OPT_ERRORS { OPT_WRNG_PARAM, OPT_INV_OPT };
typedef enum OPT_ERRORS opt_errors_t;

enum BOOLEAN { FALSE = 0, TRUE = 1 };
typedef enum BOOLEAN bool;

enum ORDER_TYPE { O_TERMINATE = 100, O_EXECUTE, O_WAITFOR };
typedef enum ORDER_TYPE o_type_t;

struct MAKEFILE{
  char *key;
  queue_t *deps;
  queue_t *comms;
  int32_t ndep, ncom, ind;
  bool visit, exec;
};
typedef struct MAKEFILE make_t;

struct ORDER{
  o_type_t type;
  int32_t key_ind;
  make_t *key;
};
typedef struct ORDER order_t;

struct THREADARGS{
  int32_t id;
};

typedef struct THREADARGS targs_t;

extern queue_t *tbd_queue;
extern pthread_t *threads;
extern pthread_attr_t stack;
extern targs_t *args;
extern sem_t *sems, queue_sem, bool_sem, mutex;
extern int32_t num_threads;
extern bool double_keys, *key_locked;

void clean_up(void);
void init_make(make_t *);
void error_opt(opt_errors_t, char, char *);
void parsed_command(char *, char *);
void parsed_new_key(char *);
void parsed_dependency(char *, char *);
void *thread_func();
bool search_key(char *, queue_t *);
bool test_cycle(const make_t *, make_t *);
void test_inexistente();
void key_request(order_t *);
void key_notify(char *);

#endif
