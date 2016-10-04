#include "ial.h"
#include "ifj.h"
#include "scaner.h"

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
/*
hTable *htableInit( unsigned long size ) {

    hTable *table = malloc( sizeof(hTable) + size * sizeof(table->data[0]) );
    if(!table)
        return NULL;

    table->size          = size;
    table->hashFunction = &hashFunction;
    table->n             = 0;

    for( unsigned int i = 0; i < size; i++ )
        table->data[i] = NULL;

    return table;

}

unsigned int hashFunction(const char *str, unsigned htab_size)
{
    unsigned int h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h % htab_size;
}

tToken * htableLookupAdd(hTable *table, const char *key)
{
    if( !table || !key )
        return NULL;

    unsigned int index = table->hashFunction(key, table->size);

    tToken* item = table->data[index];

    while(item)
    {
        if( !strcmp(item->key,key) ) 
            return item;

        else if( item->next ) 
            item = item->next;

        else 
            break;
    }


    tToken* tmp = malloc( sizeof(tToken) );
    if(!tmp)
        return NULL;

    tmp->key = malloc( sizeof( const char ) * ( strlen(key) + 1 ) );
    if(!tmp->key)
    {
        free(tmp);
        return NULL;
    }

    tmp->next = NULL;

    strcpy(tmp->key, key);

    if(item)
        item->next = tmp;

    else
        table->data[index] = tmp;

    return tmp;
}*/
