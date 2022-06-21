/*
 * linked_list.h
 *
 * Created: 15.06.2022 19:25:59
 *  Author: jesse
 */ 


#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

typedef struct node {
	void * data;
	struct node * next;
} node;

#endif /* LINKED_LIST_H_ */