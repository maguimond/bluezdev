/*
 * utils.cpp

 *
 *  Created on: 2015-11-17
 *      Author: maguimond
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void ExitOnError(const char* inFunctionSoure, const char* inErrSource, int inErrNum, const char* inFile, unsigned int inLine)
{
	printf("(%s) Erreur: %s = %s (%d)\nFile: %s\nLine: %d\n", inFunctionSoure, inErrSource, strerror(inErrNum), inErrNum, inFile, inLine);
	exit(0);
}

// --------------------------------------------------------------------------------------------------------------

void PrintStatus(const char* inFunctionSoure, const char* inMessage)
{
    printf("(%s) Status: %s\n", inFunctionSoure, inMessage);
    exit(0);
}
