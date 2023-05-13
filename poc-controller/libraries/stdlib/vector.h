/*
 * vector.h
 *
 * Created: 31/01/2021
 *  Author: Alexander Gross
 */


#ifndef VECTOR_H_
#define VECTOR_H_

#define ALLOCATION_SIZE 5 //amount of bytes allocated at once, whenever vector needs to grow

#include <avr/io.h>
#include <stdlib.h>

template<typename T>
class vector {

private:
    
    T *base, *top;          // pointer to first and behind last item
    
    size_t capacity;        // maximal available elements
    
    void grow();            // increase amount of available elements by ALLOCATION_SIZE
    
    void shrink();          // decrease amount of available elements by ALLOCATION_SIZE
    
    bool full();            // true if there is no available element
    
    bool shrinkable();      // true if the vector has at least ALLOCATION_SIZE unused memory allocated
    
public:

    vector();
    
    vector(size_t size);
    
    /**
     * Place an item behind the last item in the vector thereby growing the vector.
     */
    void push_back(T item);
    
    /**
     * Remove and return the last item in the vector thereby shrinking the vector.
     */
    T pop_back();
    
    /**
     * Access (read/write) the element of the vector at position @code index.
     * The access is not bound-checked.
     */
    T& operator[](size_t index) { return base[index]; };
    
    T* begin();
    
    T* end();
    
    /*
     * @return true if the vector is empty, otherwise false
     */
    bool empty() { return top == base; };
    
    /**
     * @ return amount of elements in the vector
     */
    size_t size() { return (top - base) * sizeof(T); };
    
    /**
     * Resets the vector to an empty state, but does not shrink it (use shrink_to_size if desired).
     */
    void clear() { top = base; }
    
    /*
     * Reduce the memory allocated by the vector (if possible) to fit exactly the size of it's items.
     */
    void shrink_to_size();
    
    /*
     * Resize the 
     */
    uint8_t resize(size_t _size);
    
    ~vector();
}; 


/************************************************************************/
/*          Definitions of templated functions                          */
/************************************************************************/


template<typename T>
inline void vector<T>::grow() {
    capacity += 5;
    // TODO: handle realloc failure
    base = (T *) realloc(base, capacity * sizeof(T));
}

template<typename T>
inline void vector<T>::shrink() {  //inline
    // only shrink of no item would get lost
    if (capacity - 5 >= size()) {
        capacity -= 5;
        // shrinks to decreased capacity
        shrink_to_size();
    }
}

template<typename T>
inline bool vector<T>::full() {
    return size() == capacity;
}

template<typename T>
inline bool vector<T>::shrinkable() {   //inline
    return capacity - size() >= ALLOCATION_SIZE;
}

template<typename T>
vector<T>::vector() : vector(ALLOCATION_SIZE) { }

template<typename T>
vector<T>::vector(size_t size) {
    // don't allocate size 0
    size = (size) ? size : ALLOCATION_SIZE;
    
    base = (T *) malloc(size * sizeof(T));
    capacity = (base) ? size : 0;
    
    top = base;
}

template<typename T>
uint8_t vector<T>::resize(size_t _size) {
    // don't allocate size 0
    _size = (_size) ? _size : ALLOCATION_SIZE;
    
    base = (T *) realloc(base, _size * sizeof(T));
    capacity = _size;
    
    // top points to end for next push
    top = base + capacity;
    
    // return size 0 on failure
    return (base) ? capacity : 0;
}

template<typename T>
void vector<T>::push_back(T item) {
    // alloc memory as needed
    if (full()) grow();
    
    *top = item;
    top++;
}

template<typename T>
T vector<T>::pop_back() {
    top--;
    T temp = *top;
    
    // free memory of possible, may invalidate end pointer
    if (shrinkable()) shrink();
    
    return temp;
}

template<typename T>
T* vector<T>::begin() {
    return base;
}

template<typename T>
T* vector<T>::end() {
    return top;
}

template<typename T>
void vector<T>::shrink_to_size() {
    // do not realloc to size 0
    capacity = (size() == 0) ? ALLOCATION_SIZE : size();
    // TODO: handle realloc failure
    base = (T *) realloc(base, capacity * sizeof(T));
    top = base + capacity*sizeof(T);
}

template<typename T>
vector<T>::~vector() {
    free(base);
}


#endif /* VECTOR_H_ */
