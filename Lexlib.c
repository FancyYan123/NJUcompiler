#include "Lexlib.h"

float exp_to_float(char* str){
	char* floatNumStr = strtok(str, "e");
	char* expNumStr = strtok(NULL, "e");

	assert((floatNumStr!=NULL) && (expNumStr!=NULL));

	float floatNum = atof(floatNumStr);
	int expNum = atoi(expNumStr);

	float step = expNum>0?10.0:0.1;
	float rtn = floatNum;
	int i;
	for(i=0; i<abs(expNum); i++){
		rtn = rtn*step;
	}

	return rtn;
}

int oct_to_deci(char* str){
	int len = strlen(str);
	int start;
	if(str[0]=='+' || str[0]=='-')
		start = 1;
	else
		start = 0;

	int rtn = 0;
	int i;
	for(i=start; i<len; i++){
		rtn = rtn*8 + (str[i]-'0');
	}
	
	if(str[0]=='-')
		rtn = -rtn;
	return rtn;
}

int hex_to_deci(char* str){
	int len = strlen(str);
	int start;
	if(str[0]=='+' || str[0]=='-')
		start = 3;
	else
		start = 2;

	int rtn = 0;
	int i;
	for(i=start; i<len; i++){
		if(str[i]>='0' && str[i]<='9')
			rtn = rtn*16 + (str[i]-'0');
		else if(str[i]>='a' && str[i]<='f')
			rtn = rtn*16 + (str[i]-'a'+10);
		else
			rtn = rtn*16 + (str[i]-'A'+10);
	}

	if(str[0]=='-')
		rtn = -rtn;
	return rtn;
}

