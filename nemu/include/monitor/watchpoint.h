#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
    int NO;
    struct watchpoint *next;
	
  /* TODO: Add more members if necessary */
	char expre[100];
	uint32_t val;
} WP;
void init_wp_pool();
void print_wp();
void free_wp(int no);
bool check_wp();
WP* new_wp();
#endif
