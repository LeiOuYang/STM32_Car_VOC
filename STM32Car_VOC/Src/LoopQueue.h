
#ifndef LOOP_QUEUE_H
#define LOOP_QUEUE_H

/***  The order structure of the queue  ***/


typedef struct{
	
	unsigned char* data;	    // the address of begin element array
	unsigned int front;				// the front point of queue
	unsigned int rear;				// the rear point of queue
	unsigned int length;
	volatile unsigned int realLen;
	
} LoopQueue; 

/*** Function declaration ***/


int destroyLoopQueue( LoopQueue* loopQueue );			    //destroy the queue, the point=NULL, all element value is 0									
int loopQueueLength( LoopQueue* loopQueue );		//get the length of queue
int loopQueueIsEmpty( LoopQueue* loopQueue );                //Determine whether the queue is empty.yes,return 1; otherwise return 0

/*** char queue ***/
int initCharLoopQueue( LoopQueue* loopQueue, unsigned char* pc, unsigned int len, unsigned char uc );	//init the queue, need to specify the length of array. all Element value is 0
int insertCharLoopQueue( LoopQueue* loopQueue, unsigned char value );					//insert element to queue
unsigned char readCharLoopQueue( LoopQueue* loopQueue );					//read element in queue
unsigned int overCountLoopQueue(LoopQueue* loopQueue);
int clearLoopQueue(LoopQueue* loopQueue);








#endif

