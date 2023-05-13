/*
 * cqueue.h
 *
 * Created: 30/01/2021
 * Author: Alexander Groﬂ
 */ 


#ifndef CQUEUE_H_
#define CQUEUE_H_

#define idx_t uint8_t  // type used for queue indexing

#include <avr/io.h>
#include <stdlib.h>

template<typename T = uint8_t>
class cqueue {

private:
    
    T* buffer;
    
    idx_t front, rear, capacity;
    
    bool _empty;

public:

    /**
     * Construct a queue object, that can hold up to @code _capacity items and acts like a FIFO.
     */
    cqueue(idx_t _capacity);
    
     /**
     * Append a the new @code item at the end of the queue. If the queue is full, the item will be dropped silently.
     */
    void push(T item);
    
    /**
     * Return the oldest item from the queue without removing it. If the queue is empty, 0 is returned.
     */
    T peek();
    
    /**
     * Return the item from the queue that was pushed @code index positions after the oldest item, where the oldest item is at position 0.
     * The item is not removed. If the queue is empty or the index is invalid, 0 is returned.
     */
    T peek(idx_t index);
    
    /**
     * Remove and return the oldest item from the queue. If the queue is empty, 0 is returned.
     */
    T pop();
    
    /**
     * Remove and return the item from the queue that was pushed @code index positions after the oldest item, where the oldest item is at position 0.
     * After removing, some queue elements are moved, to close the created gap (the order of the other items remains).
     * If the queue is empty or the index is invalid, 0 is returned.
     */
    T pop(idx_t index);
    
    /**
     * Swap the item at @code index1 with the item at @code index2.
     * If one of the indices is out of range, nothing happens. The indices begin at zero.
     */
    void swap(idx_t index1, idx_t index2);
    
    /**
     * Change the maximal amount of items the buffer can hold. If @code _capacity is less the previous capacity,
     * all items in the queue will be dropped.
     *
     * @return  0 on success, 1 otherwise (the queue is unusable after a failure)
     */
    uint8_t resize(idx_t _capacity);
    
    ~cqueue();
    
    /*** Inline funtions ***/    

    /**
     * Determine whether the queue is usable (memory is allocated).
     * If the queue is unsuable, try to allocat memory using @code resize().
     *
     * @return  true if the queue is usable, false otherwise
     */
    bool usable() { return buffer != nullptr; };
    
    /**
     * Delete all items in the queue (sets the size to zero).
     */
    void clear() { _empty = true; };

    /**
     * Check whether the queue contains any items.
     *
     * @return  true if there are no items in the queue, false otherwise
     */
    bool empty() { return _empty; };

    /**
     * Check whether the memory allocated for the queue is completely used (i.e. the next pushed item will be dropped).
     * More memory can be allocated using the @code resize() method.
     *
     * @return  true if no more items can be pushed, false otherwise
     */
    bool full() { return (front == 0 && rear + 1 == capacity) || (front == rear + 1); };

    /**
     * Get the amount of items in the queue.
     *
     * @return  amount of items in the queue
     */
    idx_t size() { return (_empty) ? 0 : (front > rear)? (rear + (capacity - front) + 1) : (rear - front + 1); };
        
    /**
     * Get the amount of available item "slots" in the queue.
     *
     * @return  amount of items to push until queue is full (if nothing is popped)
     */
    idx_t available() { return (_empty) ? capacity : capacity - size(); };
};


/************************************************************************/
/*          Definitions of templated functions                          */
/************************************************************************/


template<typename T>
cqueue<T>::cqueue(idx_t _capacity) : front(0), rear(0), _empty(true) {
    // allocate at least one element
    if (_capacity == 0) 
	{
		_capacity = 1;
	}
    this->buffer = (T *) malloc(_capacity*sizeof(T)); // may return nullptr
    this->capacity = _capacity;
};

template<typename T>
void cqueue<T>::push(T item) {
    if (this->full())
	{
    // overflow, ignore item
        return;
	}
    // (re)init queue if empty
    if (this->_empty) {
        this->front = this->rear = 0;   // reset pointer
        this->_empty = false;
    } else {
        // reset rear to first position if end is reached
        this->rear++;
        if(this->rear == this->capacity)
            this->rear = 0;
            
    }
    this->buffer[this->rear] = item;
}

template<typename T>
T cqueue<T>::peek() {
    if (this->_empty)
    {
        // queue is empty
        return 0;
    }
    
    return buffer[front];
}

template<typename T>
T cqueue<T>::peek(idx_t index) {
    // index is zero based
    if (index == 0) {
        return peek();
    }
    // validate index
    else if (index >= size() + 1) {
        // queue is not large enough
        return 0;
    }
    
    // offset indices with front (mod capacity), size_t to prevent overflow
    size_t pos = front + index;
    if (pos >= capacity) pos -= capacity;
    
    return buffer[pos];
}

template<typename T>
T cqueue<T>::pop() {
    if (this->_empty)
	{
        // queue is empty
        return 0;
	}
    T returnValue = buffer[front];
    if (this->front == this->rear) {
        // only one element -> empty queue
        _empty = true;
    } else {
        // reset front to first position if end is reached
        this->front++;
        if (this->front == this->capacity)
		    this->front = 0;
    }
    return returnValue;
}

template<typename T>
T cqueue<T>::pop(idx_t index) {
    // index is zero based
    if (index == 0) {
        return pop();
    }
    // validate index
    else if (index >= size() + 1) {
        // queue is not large enough
        return 0;
    }
    
    // offset indices with front (mod capacity), size_t to prevent overflow
    size_t pos = front + index;
    if (pos >= capacity) pos -= capacity;
    
    T returnValue = buffer[pos];
    
    /*
     * Because the above code validates the index and implements index = 0 by calling pop_old(),
     * at this program point, there will be more than one element in the queue.
     * Therefore we have to shift items to close the "gap" in the queue and increase the front pointer once.
     */
    idx_t i;
    if (pos < front) {
        // the pointer wrapped around
        for (i = pos; i > 0; i--) {
            buffer[i] = buffer[i-1];
        }
        // wrap item around buffer
        buffer[0] = buffer[capacity-1];
        
        for (i = capacity-i; i > front; i--) {
            buffer[i] = buffer[i-1];
        }
        
    } else {
        // straight queue layout
        for (i = pos; i > front; i--) {
            buffer[i] = buffer[i-1];
        }
    }
    
    // reset front to first position if end is reached
    this->front++;
    if (this->front == this->capacity)
    this->front = 0;
    
    return returnValue;
}

template<typename T>
void cqueue<T>::swap(idx_t index1, idx_t index2) {
    // validate indices
    if (index1 >= size() || index2 >= size()) {
        return;
    } else {
        // offset indices with front (mod capacity), size_t to prevent overflow
        size_t i1 = front + index1;
        size_t i2 = front + index2;
        if (i1 >= capacity) i1 -= capacity;
        if (i2 >= capacity) i2 -= capacity;
        
        // swap
        T temp = buffer[i1];
        buffer[i1] = buffer[i2];
        buffer[i2] = temp;
    }
}

template<typename T>
uint8_t cqueue<T>::resize(idx_t _capacity) {
    if (_capacity == 0)
	{
		_capacity = 1;
	}
    else if (_capacity == this->capacity)
	{
		return 0;
	}
    
    T* new_buff = (T *) realloc(this->buffer, _capacity*sizeof(T));
    if (new_buff != nullptr) {
        // clear queue if capacity shrinks
        if (this->capacity > _capacity)
        {
			_empty = true;
		}
        this->capacity = _capacity;
        this->buffer = new_buff;
        
        return 0;
    } else {
        // realloc failed, deallocate old pointer
        free(this->buffer);
        this->buffer = nullptr;
        
        return 1;
    }
}

template<typename T>
cqueue<T>::~cqueue() {
    free(this->buffer);   // nullptr save
}


#endif /* CQUEUE_H_ */
