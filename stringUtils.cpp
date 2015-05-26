#include <string.h>  //strcop and strcat
#include <stdlib.h>  //mbstowcs

wchar_t* char2wchar_t(char* inputString){
	size_t size = strlen(inputString) + 1;
	wchar_t* result = new wchar_t[size];
	mbstowcs(result,inputString,size);
	return result;
	}

char* combine(char* in1, char* in2){
	char result[100];
	strcpy (result,in1);
	strcat (result,in2);
	return result;
	}

wchar_t* constChar2wchar_t(const char* inputString){
	size_t size = strlen(inputString) + 1;
	wchar_t* result = new wchar_t[size];
	mbstowcs(result,inputString,size);
	return result;
	}