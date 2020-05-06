#include<stdio.h>
#include "monitor/expr.h"
int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {	
    /* Initialize the monitor. */
    int is_batch_mode = init_monitor(argc, argv);
	/*
	unsigned int ans1;
	char s[66666];
	FILE *fp =NULL;
	if( (fp = fopen("./tools/gen-expr/input", "r")) == NULL)
		printf("File cannot open.\n");
	int i = 0;
	while(~fscanf(fp, "%u", &ans1)){
		bool *success, flag = 1;
		success = &flag;
		fscanf(fp, "%[^\n]%*c", s);
		printf("No. %d : ", i);
		++i;
		if(ans1 == expr(s, success))
			printf("True.\n");
		else
			printf("False. Ying Ying Ying\n");
	}
	fclose(fp); 	
	*/
    /* Receive commands from user. */
    ui_mainloop(is_batch_mode);
    return 0;
}
