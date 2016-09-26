#include "ifj.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

	if(argc != 2) {
		fprintf(stderr, "Input file is not specified\n");
		return FILE_ERROR;
	}

	FILE *fp;
	if(!( fp = fopen(argv[1], "r") ) ) {
		fprintf(stderr, "Unable to open input file '%s'\n", argv[1]);
		return FILE_ERROR;
	}




	return SUCCESS;
}