#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main () 
{ 
    int num1, num2;
 	/* use scanf to get input from console */ 
 	scanf("%d %d", &num1, &num2);
	/* use printf to print output to console */ 
  printf("%d", num1 + num2);
 
    return(0); 
}

