#ifndef __LRU_HPP__
#define __LRU_HPP__

#include <list>
#include <unordered_map>

/* Least Recently Used cache buffer implementation.
 * All operations are O(1).
 */
template <typename T>
class LRUBuffer{
public:
    LRUBuffer(unsigned int maxsize) : maxsize_(maxsize) {}
    unsigned int size() const {return elements_.size();}
    void Use(T elem){
        // Test if this element already exists in the buffer
        if(elem_map_.count(elem) > 0){
            // Erase it from the current position
            typename std::list<T>::iterator it = elem_map_[elem];
            elements_.erase(it);
        }
        // Add the element to the head of the list
        auto it = elements_.insert(elements_.begin(), elem);
        // Place the iterator in the map
        elem_map_[elem] = it;
        // If the buffer is too large now
        if(elements_.size() > maxsize_){
            // Erase the least recently used element
            elem_map_.erase(elements_.back());
            elements_.pop_back();
        }
    }
    typename std::list<T>::iterator begin() {return elements_.begin();}
    typename std::list<T>::iterator end() {return elements_.end();}
private:
    unsigned int maxsize_;
    std::list<T> elements_;
    std::unordered_map<T, typename std::list<T>::iterator> elem_map_;
};

#endif // __LRU_HPP__
