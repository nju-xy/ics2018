#include "klib.h"
#include <stdarg.h>

//#ifndef __ISA_NATIVE__
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
	int ret = 0;
	va_list ap;
	va_start(ap, *fmt);
    for(; *fmt != '\0'; fmt++){
        if(*fmt != '%'){
            _putc(*fmt);
			ret++;
        }
        else {
			int	width = 0;
			fmt++;
			if(*fmt == '0') {
				fmt++;		  
				while(*fmt <= '9' && *fmt >= '0'){
					width = width * 10 + *fmt - '0';
					fmt++;
				}
			}
            switch (*fmt){
                case 's': {
                    char * s = va_arg(ap, char*);
                    while(*s != '\0') {
                        _putc(*s);
						ret++;
                        s++;
                    }
					//_putc(*s);
					//ret++;
                    break;
                }
                case 'd': {
                    int d = va_arg(ap, int);
                    char s[20];
                    int l;
					if(d < 0)
					{
						if(d == -2147483648){
							_putc('-');
							_putc('2');
							_putc('1');
							_putc('4');
							_putc('7');
							_putc('4');
							_putc('8');
							_putc('3');
							_putc('6');
							_putc('4');
							_putc('8');
							ret += 11;
							break;
						}
						d = -d;
						_putc('-');
						ret++;
					}
					if(width == 0){
						if(d == 0){
							_putc('0');
							ret++;
							break;
						}
						for(l = 0; d != 0; ++l){
							s[l] = '0' + d % 10;
							d /= 10;
						}
					}
					else{
						for(l = 0; l < width; ++l){
							s[l] = '0' + d % 10;
							d /= 10;
						}
					}
                    l--;
                    while(l >= 0){
						_putc(s[l]);
						ret++;
						l--;
                    }
					break;
                }
				case 'u': {
                    unsigned int d = va_arg(ap, unsigned int);
                    char s[20];
                    int l;
					if(width == 0){
						if(d == 0){
							_putc('0');
							ret++;
							break;
						}
						for(l = 0; d != 0; ++l){
							s[l] = '0' + d % 10;
							d /= 10;
						}
					}
					else{
						for(l = 0; l < width; ++l){
							s[l] = '0' + d % 10;
							d /= 10;
						}
					}
                    l--;
                    while(l >= 0){
						_putc(s[l]);
						ret++;
						l--;
                    }
					break;
                }
				case 'x':{
                    int d = va_arg(ap, int);
                    char s[20];
                    int l;
					if(d < 0)
					{
						if(d == -2147483648){
							_putc('-');
							_putc('1');
							_putc('0');
							_putc('0');
							_putc('0');
							_putc('0');
							_putc('0');
							_putc('0');
							_putc('0');
							ret += 9;
							break;
						}
						d = -d;
						_putc('-');
						ret++;
					}
					if(width == 0){
						if(d == 0){
							_putc('0');
							ret++;
							break;
						}
						for(l = 0; d != 0; ++l){
							s[l] = '0' + d % 16;
							d /= 16;
						}
					}
					else{
						for(l = 0; l < width; ++l){
							s[l] = '0' + d % 16;
							d /= 16;
						}
					}
                    l--;
                    while(l >= 0){
						if(s[l] > '9'){
							s[l] = s[l] - '9' - 1 + 'a';
						}
						_putc(s[l]);
						ret++;
						l--;
                    }
					break;
                }
				case 'p':
				{
					unsigned int* p = va_arg(ap, void*);
					ret += printf("%u", p);
					break;
                }
				case '%':{ _putc('%'); break; }
				default: _putc('T'); _putc('O'); _putc('D'); _putc('O'); break;
             }
         }
    } 
    va_end(ap);
    return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
	char *str;
    for(str = out; *fmt != '\0'; fmt++){
        if(*fmt != '%'){
            *str = *fmt;
			str++;
        }
        else {
			int width = 0;
            fmt++;
			if(*fmt == '0') {
				fmt++;		  
				while(*fmt <= '9' && *fmt >= '0'){
					width = width * 10 + *fmt - '0';
					fmt++;
				}
			}
            switch (*fmt){
                case 's': {
                    char * s = va_arg(ap, char*);
                    while(*s != '\0') {
                        *str = *s;
                        s++;
						str++;
                    }
					//*str = *s;
                    break;
                }
                case 'd': {
                    int d = va_arg(ap, int);
                    char s[12];
                    int l;
					if(d < 0){
						if(d == -2147483648){
							*str++ = '-';
							*str++ = '2';
							*str++ = '1';
							*str++ = '4';
							*str++ = '7';
							*str++ = '4';
							*str++ = '8';
							*str++ = '3';
							*str++ = '6';
							*str++ = '4';
							*str++ = '8';
							break;
						}
						d = -d;
						*str++ = '-';
					}
                    if(width == 0){
						if(d == 0){
                        	*str = '0';
							str++;
                        	break;
                   		}
                    	for(l = 0; d != 0; ++l){
                    	    s[l] = '0' + d % 10;
							d /= 10;
                   		}					
					}
					else{
						for(l = 0; l < width; ++l){
                    	    s[l] = '0' + d % 10;
							d /= 10;
                   		}					
					}
                    l--;
                    while(l >= 0){
						*str = s[l];
						l--;
						str++;
                    }
					break;
                }
				case 'u': {
                    unsigned int d = va_arg(ap, unsigned int);
                    char s[12];
                    int l;
                    if(width == 0){
						if(d == 0){
                        	*str = '0';
							str++;
                        	break;
                   		}
                    	for(l = 0; d != 0; ++l){
                    	    s[l] = '0' + d % 10;
							d /= 10;
                   		}					
					}
					else{
						for(l = 0; l < width; ++l){
                    	    s[l] = '0' + d % 10;
							d /= 10;
                   		}					
					}
                    l--;
                    while(l >= 0){
						*str = s[l];
						l--;
						str++;
                    }
					break;
                }
				case 'x': {
                    int d = va_arg(ap, int);
                    char s[12];
                    int l;
					if(d < 0)
					{
						if(d == -2147483648){
							*str++ = '-';
							*str++ = '1';
							*str++ = '0';
							*str++ = '0';
							*str++ = '0';
							*str++ = '0';
							*str++ = '0';
							*str++ = '0';
							*str++ = '0';
							break;
						}
						d = -d;
						*str++ = '-';
					}
                    if(width == 0){
						if(d == 0){
                        	*str = '0';
							str++;
                        	break;
                   		}
                    	for(l = 0; d != 0; ++l){
                    	    s[l] = '0' + d % 16;
							d /= 16;
                   		}					
					}
					else{
						for(l = 0; l < width; ++l){
                    	    s[l] = '0' + d % 16;
							d /= 16;
                   		}					
					}
                    l--;
                    while(l >= 0){
						*str = s[l];
						l--;
						str++;
                    }
					break;
                }
                default: _putc('T'); _putc('O'); _putc('D'); _putc('O'); break;
            }
        }
    }
    va_end(ap);
	*str = '\0';
    return (int)(str - out);
}

int sprintf(char *out, const char *fmt, ...) {
    char *str;
	va_list ap;
	va_start(ap, *fmt);
    for(str = out; *fmt != '\0'; fmt++){
        if(*fmt != '%'){
            *str = *fmt;
			str++;
        }
        else {
			int width = 0;
            fmt++;
			if(*fmt == '0') {
				fmt++;		  
				while(*fmt <= '9' && *fmt >= '0'){
					width = width * 10 + *fmt - '0';
					fmt++;
				}
			}
            switch (*fmt){
                case 's': {
                    char * s = va_arg(ap, char*);
                    while(*s != '\0') {
                        *str = *s;
                        s++;
						str++;
                    }
					//*str = *s;
                    break;
                }
                case 'd': {
                    int d = va_arg(ap, int);
                    char s[12];
                    int l;
					if(d < 0){
						if(d == -2147483648){
							*str++ = '-';
							*str++ = '2';
							*str++ = '1';
							*str++ = '4';
							*str++ = '7';
							*str++ = '4';
							*str++ = '8';
							*str++ = '3';
							*str++ = '6';
							*str++ = '4';
							*str++ = '8';
							break;
						}
						d = -d;
						*str++ = '-';
					}
                    if(width == 0){
						if(d == 0){
                        	*str = '0';
							str++;
                        	break;
                   		}
                    	for(l = 0; d != 0; ++l){
                    	    s[l] = '0' + d % 10;
							d /= 10;
                   		}					
					}
					else{
						for(l = 0; l < width; ++l){
                    	    s[l] = '0' + d % 10;
							d /= 10;
                   		}					
					}
                    l--;
                    while(l >= 0){
						*str = s[l];
						l--;
						str++;
                    }
					break;
                }
				case 'u': {
                    unsigned int d = va_arg(ap, unsigned int);
                    char s[12];
                    int l;
                    if(width == 0){
						if(d == 0){
                        	*str = '0';
							str++;
                        	break;
                   		}
                    	for(l = 0; d != 0; ++l){
                    	    s[l] = '0' + d % 10;
							d /= 10;
                   		}					
					}
					else{
						for(l = 0; l < width; ++l){
                    	    s[l] = '0' + d % 10;
							d /= 10;
                   		}					
					}
                    l--;
                    while(l >= 0){
						*str = s[l];
						l--;
						str++;
                    }
					break;
                }
				case 'x': {
                    int d = va_arg(ap, int);
                    char s[12];
                    int l;
					if(d < 0)
					{
						if(d == -2147483648){
							*str++ = '-';
							*str++ = '1';
							*str++ = '0';
							*str++ = '0';
							*str++ = '0';
							*str++ = '0';
							*str++ = '0';
							*str++ = '0';
							*str++ = '0';
							break;
						}
						d = -d;
						*str++ = '-';
					}
                    if(width == 0){
						if(d == 0){
                        	*str = '0';
							str++;
                        	break;
                   		}
                    	for(l = 0; d != 0; ++l){
                    	    s[l] = '0' + d % 16;
							d /= 16;
                   		}					
					}
					else{
						for(l = 0; l < width; ++l){
                    	    s[l] = '0' + d % 16;
							d /= 16;
                   		}					
					}
                    l--;
                    while(l >= 0){
						*str = s[l];
						l--;
						str++;
                    }
					break;
                }
                default: _putc('T'); _putc('O'); _putc('D'); _putc('O'); break;
            }
        }
    }
    va_end(ap);
	*str = '\0';
    return (int)(str - out);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
	char *str = NULL;
	va_list ap;
	va_start(ap, fmt);
	int ret = vsprintf(str, fmt, ap);
    va_end(ap);
	
	strncpy(out, str, n);
	if(ret < n)  return ret;
	else return n;
	//return n;
}

#endif
