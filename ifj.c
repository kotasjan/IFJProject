#include "ifj.h"
#include "ial.h"
#include "scaner.h"

FILE *fp;

int main(int argc, char **argv){

	int ret;

	if(argc != 2) {
		fprintf(stderr, "Input file is not specified\n");
		return FILE_ERROR;
	}

	
	if(!( fp = fopen(argv[1], "r") ) ) {
		fprintf(stderr, "Unable to open input file '%s'\n", argv[1]);
		return FILE_ERROR;
	}

	setFile(fp);

	ret = parse();

	printErrCode(ret);


	return ret;
}