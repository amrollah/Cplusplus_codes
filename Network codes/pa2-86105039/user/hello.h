/*
 * hello.h
 *
 *  Created on: May 7, 2010
 *      Author: amrollah
 */

#ifndef HELLO_H_
#define HELLO_H_

class hello{
public:
	int rootID;
	int ID;
	int dist;
	long int age;
	hello(int,int,int,long int);
	~hello();
};

hello::hello(int rootID,int ID,int dist,long int age){
	ID = ID;
	rootID = rootID;
	dist = dist;
	age = age;
}

#endif /* HELLO_H_ */
