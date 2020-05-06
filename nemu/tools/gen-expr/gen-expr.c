#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];

static inline uint32_t choose(uint32_t n){
	uint32_t temp = rand();
	temp %= n;
	if(temp < 0)  temp += n;
	return temp;
}

static inline void gen_num(){
	if(strlen(buf) > 65530)
		return;
	char s[20] = "\0";
	uint32_t temp = choose(1000);
	sprintf(s, "%u", temp);
	strcat(s, "U");
	strcat(buf, s);
	return;
}

static inline void gen_rand_op(){
	if(strlen(buf) > 65530)  return;
	switch(choose(4)){
		case 0: strcat(buf, "+");  break;
		case 1: strcat(buf, "-");  break;
		case 2: strcat(buf, "*");  break;
		default: strcat(buf, "/"); break;
	}
	return;
}

static inline void gen(char *s){
	if(strlen(buf) > 65530)  return;
	strcat(buf, s);
}

static inline void gen_blank(){
	if(strlen(buf) > 65530)  return;
	unsigned int temp = choose(3);
	for(int i = 0; i < temp; ++i)
		strcat(buf, " ");
}

static inline void gen_rand_expr() {
	if(strlen(buf) > 65530)  return;
    //buf[0] = '\0';
	switch(choose(3)){
		case 0: gen_blank(); 
				gen_num(); 
				gen_blank();
				break;
		case 1: gen_blank(); 
				gen("(");
				gen_blank();	
				gen_rand_expr();
				gen_blank();
				gen(")");
				gen_blank();
				break;
		default:gen_blank();
				gen_rand_expr();
				gen_blank();
			   	gen_rand_op();
				gen_blank();
			   	gen_rand_expr(); 
				gen_blank();
				break;
	}
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    //buf[0] = '\0';
	memset(buf,0,sizeof(buf)/sizeof(char));
	buf[0] = '\0';
	gen_rand_expr();
	if(strlen(buf) >= 65530){
		printf("0 0\n");
		memset(buf,0,sizeof(buf)/sizeof(char));
		continue;
		//printf("The array buf may overflow.\n");
		//memset(buf,0,sizeof(buf)/sizeof(char));
		//strcat(buf, "0");
		//assert(0);
	}
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen(".code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc .code.c -o .expr");
    if (ret != 0) continue;

    fp = popen("./.expr", "r");
    assert(fp != NULL);

    uint32_t result;
    fscanf(fp, "%u", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
