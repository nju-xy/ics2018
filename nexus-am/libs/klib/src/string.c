#include "klib.h"
//#include<assert.h>

//#ifndef __ISA_NATIVE__
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	//if(s == NULL)  assert(0);
	int len = 0;
	while((*s++) != '\0'){
		len++;
	}
	return len;
}

char *strcpy(char* dst,const char* src) {
	//if((dst == NULL) || (src == NULL))  assert(0);
	char* strDestCopy = dst;
	while(*src != '\0'){
		*dst++ = *src++;
	}
	*dst = '\0';
	return strDestCopy;
}

char* strncpy(char* dst, const char* src, size_t n) {
	//if((dst == NULL) || (src == NULL))  assert(0);
	/*char* strDestCopy = dst;
	while(n && *src != '\0' ){
		*dst++ = *src++;
		n--;		
	}
	if(n != 0){
		*dst++ = '\0';
		n--;
	}
	return strDestCopy;*/
	size_t i;
	for(i = 0; i < n && src[i] != '\0'; i++) 
		dst[i] = src[i];
	for(; i < n; i++) 
		dst[i] = '\0';
	return dst;
}

char* strcat(char* dst, const char* src) {
	//if((dst == NULL) || (src == NULL))  assert(0);
	char* str = dst;
	while(*dst != '\0'){
		dst++;
	}
	while(*src != '\0'){
		*dst++ = *src++;
	}
	*dst = *src;
	return str;
}

int strcmp(const char* s1, const char* s2) {	
	//if((s1 == NULL) || (s2 == NULL))  assert(0);
	while(1){
		int temp = *(unsigned char*)s1 - *(unsigned char*)s2;
		if(temp < 0)  return -1;
		else if(temp > 0) return 1;
		if(*s1 == '\0')  break;
		s1++;
		s2++;
	}
	return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
	//if((s1 == NULL) || (s2 == NULL))  assert(0);
	while(n){
		n--;
		int temp = *(unsigned char*)s1 - *(unsigned char*)s2;
		if(temp < 0)  return -1;
		else if(temp > 0)  return 1;
		if(*s1 == '\0')  break;
		s1++;
		s2++;
	}
	return 0;
}

void* memset(void* v,int c,size_t n) {
	//if((n < 0) || (v == NULL))  assert(0);
	unsigned char uc = c;
	unsigned char *uv = v;
	while(n){
		n--;
		*uv++ = uc;
	}
	return v;
}

void* memcpy(void* out, const void* in, size_t n) {
	//if((n < 0) || (out == NULL) || (in == NULL))  assert(0);
	void* ret = out;
	while(n){
		n--;
		*(char*)out++ = *(char*)in++;
	}
	return ret;
}

int memcmp(const void* s1, const void* s2, size_t n){	
	//if((n < 0) || (s1 == NULL) || (s2 == NULL))  assert(0);
	while(n){
		int temp = (*(unsigned char*)s1++) - (*(unsigned char*)s2++);
		n--;
		if(temp > 0)  return 1;
		if(temp < 0)  return -1;
	}
	return 0;
}

#endif
