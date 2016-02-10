/*
 * utils.h
 *
 *  Created on: 2015-11-17
 *      Author: maguimond
 */

#ifndef UTIL_H_
#define UTIL_H_

void ExitOnError(const char* inFunctionSoure, const char* inErrSource, int inErrNum, const char* inFile, unsigned int inLine);
void PrintStatus(const char* inFunctionSoure, const char* inMessage);

#endif /* UTIL_H_ */
