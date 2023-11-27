/*
 * fetch.h
 *
 *  Created on: 9 Nov 2020
 *      Author: Raul Rosa
 */

#ifndef _FETCH_H_
#define _FETCH_H_

typedef struct{

	char* key;
	char* value;

} Header;

typedef enum{
	GET,
	POST,
	PATCH
}HttpMethod;

struct FetchParams{
	void (*onGotData)(char* data);
	int headerCount;
	Header header[5];
	HttpMethod method;
	char* body;
	int status;
};



void fetch(char* url, struct FetchParams *fetchparams);


#endif /* MAIN_FETCH_H_ */
