#include "ial.h"

int length(char *s) {

	return strlen(s);

}

char *substr(char *s, int i, int n) {

	if( strlen(s) < (i + n) )
		return NULL;

	char *str = malloc( (n + 1) * sizeof(char) );
	if(!str)
		return NULL;

	return memcpy( str, s + i, n );

}

int compare(char *s1, char *s2) {

    int i = 0;
    while ( s1[i] != '\0' ) {
    	
        if( s2[i] == '\0' ) return 1; 
        else if( s1[i] > s2[i] ) return 1; 
        else if( s1[i] < s2[i] ) return -1; 

        i++;
    } 

    return 0;

}

int find(char *s, char *search) {

}

char *sort(char *s) {
	
}

int readInt() {

}