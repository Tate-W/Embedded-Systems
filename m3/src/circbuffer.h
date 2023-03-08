#ifndef CIRCBUFFER_H
    #define CIRCBUFFER_h
    #include <assert.h>
    #include <stdbool.h>


typedef struct circbuffer{
    char* buffer;
    uint8_t headIndex;
    uint8_t tailIndex;
    bool isFull;
    uint8_t max;
}circbuffer;

void circbuffer_reset(circbuffer* myBuff){
    assert(myBuff);

    myBuff->headIndex = 0;
    myBuff->tailIndex = 0;
    myBuff->isFull = false;
}

bool bufferIsEmpty(circbuffer* myBuff ){
    assert(myBuff);

    return(!myBuff->isFull && (myBuff->headIndex == myBuff->tailIndex));
}

void circbuffer_init(circbuffer* myBuff, char* buff, int size){
    assert(myBuff && buff && size);
    
    myBuff->buffer = buff;
    myBuff->max = size;
    circbuffer_reset(myBuff);

    assert(bufferIsEmpty(myBuff));
}

bool bufferIsFull(circbuffer* myBuff){
    assert(myBuff);
    return myBuff->isFull;
}

uint8_t circbuff_entries(circbuffer* myBuff){
    assert(myBuff);

	uint8_t size = myBuff->max;

	if(!myBuff->isFull){
	
    	if(myBuff->headIndex >= myBuff->tailIndex)
			size = myBuff->headIndex - myBuff->tailIndex;
		else
			size = myBuff->max + myBuff->headIndex - myBuff->tailIndex;
	}
	
    return size;
}


static void advancePointer(circbuffer* myBuff){
    assert(myBuff);

    if(myBuff->isFull){
        if(++(myBuff->headIndex) == myBuff->tailIndex){
            myBuff->tailIndex = 0;
        }
    }

    if(++(myBuff->headIndex) == myBuff->max){
        myBuff->headIndex = 0;
    }

    myBuff->isFull = (myBuff->headIndex == myBuff->tailIndex);
}

static void backPointer(circbuffer* myBuff){
    assert(myBuff);

    myBuff->isFull = false;

    if(++(myBuff->tailIndex) == myBuff->max){
        myBuff->tailIndex = 0;
    }
}

void circbuffer_queue(circbuffer* myBuff, char data){
    assert(myBuff && myBuff->buffer);

    myBuff->buffer[myBuff->headIndex] = data;

    advancePointer(myBuff);
}

bool circbuffer_dequeue(circbuffer* myBuff, char* data){
    assert(myBuff);
    assert(data);
    assert(myBuff->buffer);

    bool x = false;

    if(!bufferIsEmpty(myBuff)){
        *data = myBuff->buffer[myBuff->tailIndex];
        backPointer(myBuff);
        x = true;
    }

    return x;
}

bool circbuff_recent(circbuffer* myBuff, char* data){
    assert(myBuff && data && myBuff->buffer);

    bool x = false;

    if(!bufferIsEmpty(myBuff)){

        if(myBuff->headIndex == 0){
            myBuff->headIndex = (myBuff->max) - 1;
            *data = myBuff->buffer[myBuff->headIndex];
            x = true;	
        }   
        else{
            *data = myBuff->buffer[--(myBuff->headIndex)];
            x = true;
        }
    }

    return x;
}

#endif