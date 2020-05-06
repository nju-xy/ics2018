#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
    int i;
    for (i = 0; i < NR_WP; i ++) {
        wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
    }
    wp_pool[NR_WP - 1].next = NULL;

    head = NULL;
    free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(){
	WP *wp = free_;
	if(free_ == NULL){
		printf("The wp_pool is full.\n");
		assert(0);
	}
	free_ = free_->next;
	if(head == NULL){  
		head = wp; 
		wp->next = NULL;
	}
	else{
		wp->next = head;
		head = wp;
	}
	return wp;
}
void free_wp(int no){
	WP *wp = head;
	WP *wp_before = NULL;
	while(wp){
		if(wp->NO != no){
			wp_before = wp;
			wp = wp->next;
		}
		else break;
	}
	if(wp == NULL){
		printf("There is no such watchpoint.\n");
		return;
	}
	if(wp_before == NULL){
		head = wp->next;
	}
	else if(wp->next == NULL){
		wp_before->next = NULL;
	}
	else{
		wp_before->next = wp->next;
	}
	wp->next = free_;
	free_ = wp;	
}
void print_wp(){
	WP *wp = head;
	if(wp == NULL){
		printf("No watchpoints.\n");
	}
	else{
		while(wp){
			printf("\033[32mWatchpoint NO.%d: %s %u\n", wp->NO, wp->expre, wp->val);
			wp = wp->next;
		}
	}
}

bool check_wp(){
	bool flag = 1;
	WP *wp = head;
	while(wp != NULL){
		bool success = 1;
		uint32_t val2 = expr(wp->expre, &success);
		if(success == 0){
			printf("The expression of watchpoint %d is wrong.", wp->NO);
			flag = 0;
		}
		else if(wp->val != val2){
			printf("The watchpoint NO.%d: %s  is changed from %u to %u\n", wp->NO, wp->expre, wp->val, val2);
			wp->val = val2;
			flag = 0;
		}
		wp = wp->next;
	}
	return flag;
}
