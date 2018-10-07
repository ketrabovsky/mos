#include <my_stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#include <usart.h>

static void my_putc(char c)
{
	while(!(USART2->SR & (1 << 6))) {}
	USART_Transmit(USART2, c);
}

uint32_t _strlen(char *str)
{
	uint32_t i = 0;

	while(*str++)
	{
		i++;
	}

	return i;
}

static void reverse(char str[])
{
	char buffer[100];
	uint32_t i = 0;	
	uint32_t length = _strlen(str);
	uint32_t __length = length;
		
	while(i < length)
	{
		__length--;
		buffer[__length] = str[i];
		i++;	
	}
	
	i = 0;

	while(str[i] != '\0')
	{
		str[i] = buffer[i];
		i++;
	}
}

static char* _itoa(int num, char *str, uint32_t base)
{
	
    uint32_t i = 0;
    bool isNegative = false;
 
    /* Handle 0 explicitely, otherwise empty string is pruint32_ted for 0 */
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
 
    // In standard itoa(), negative numbers are handled only with 
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {

        int rem = num % base;
        str[i++] = (rem > 9)? (rem-10) + 'A' : rem + '0';
        num = num / base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str);
 
    return str;
}

static void printNumber(uint32_t number, uint32_t base)
{
	char buffer[20];			
	char *pointer;
	pointer = _itoa(number, buffer, base);	
	while(*pointer)
	{
		my_putc(*pointer);
		pointer++;
	}
}


uint32_t _printf(const char* format, ...)
{
    va_list parameters;
    va_start(parameters, format);

    char c;	

    while((c = *format++) != '\0')
    {
        if(c == '%')
        {
            c = *format++;
            switch(c)
            {
                case 's':
                    {
						/* Case s for printing strings */
                        char *t = (char *)va_arg(parameters, uint32_t);
                        while(*t)
                        {
                            my_putc(*t);
                            t++;
                        } break;
                    }
                case 'c':
                    {
						/* Case c for printing single characters */
                        my_putc((char)va_arg(parameters, uint32_t));	
                    } break;
                case 'd':
                    {
						/* Case d for printing integers */
						printNumber((uint32_t)va_arg(parameters, uint32_t), 10);
                    } break;
                case 'x':
                    {
						/* Case x for printing integers in hexadecimal format */
						printNumber((uint32_t)va_arg(parameters, uint32_t), 16);
                    } break;
            }
        }
        else
        {
            my_putc(c);
        }
    }

    return 0;
}

