#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <utility>

#include "array_ptr.h"

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    SimpleVector(const SimpleVector& other) {
    	*this = other;
    }

	SimpleVector(SimpleVector&& other) {
		*this = std::move(other);
		other.capacity_ = 0;
		other.size_ = 0;
	}

	SimpleVector& operator=(const SimpleVector& rhs) {
		size_t otherSize = rhs.GetSize();
		ArrayPtr<Type> temp(otherSize);
		std::copy(rhs.begin(), rhs.end(), temp.Get());
		items_.Swap(temp);
		size_ = otherSize;
		capacity_ = otherSize;
		return *this;
	}

	SimpleVector& operator=(SimpleVector&& rhs) {
		size_t otherSize = rhs.GetSize();
		ArrayPtr<Type> temp(otherSize);
		std::move(rhs.begin(), rhs.end(), temp.Get());
		items_.Swap(temp);
		size_ = otherSize;
		capacity_ = otherSize;
		return *this;
	}

	explicit SimpleVector(size_t size) {
		ArrayPtr<Type> temp(size);
		std::fill(temp.Get(), (temp.Get() + size), 0);
		items_.Swap(temp);
		size_ = size;
		capacity_ = size;
	}

	SimpleVector(size_t size, const Type& value) {
		ArrayPtr<Type> temp(size);
		std::fill(temp.Get(), (temp.Get() + size), value);
		items_.Swap(temp);
		size_ = size;
		capacity_ = size;
	}

	SimpleVector(std::initializer_list<Type> init) {
		size_ = init.size();
		ArrayPtr<Type> temp(size_);
		std::copy(init.begin(), init.end(), temp.Get());
		items_.Swap(temp);
		capacity_ = (size_);
	}

	void PushBack(const Type& item) {
		if(size_ >= capacity_){
			if(capacity_ == 0){
				capacity_ = 1;
			}
			ArrayPtr<Type> temp(capacity_ * 2);
			std::copy(items_.Get(), (items_.Get() + size_), temp.Get());
			temp[size_] = item;
			std::fill((temp.Get() + size_ + 1), (temp.Get() + capacity_ * 2), 0);
			items_.Swap(temp);
			++size_;
			capacity_ = capacity_ * 2;
		}else{
			items_[size_] = item;
			size_++;
		}
	}

	void PushBack(Type&& item) {
		if(size_ >= capacity_){
			if(capacity_ == 0){
				capacity_ = 1;
			}
			ArrayPtr<Type> temp(capacity_ * 2);
			std::move(items_.Get(), (items_.Get() + size_), temp.Get());
			temp[size_] = std::move(item);
			std::fill((temp.Get() + size_ + 1), (temp.Get() + capacity_ * 2), 0);
			items_.Swap(temp);
			++size_;
			capacity_ = capacity_ * 2;
		}else{
			items_[size_] = std::move(item);
			size_++;
		}
	}

	Iterator Insert(ConstIterator pos, const Type& value) {
		assert(pos >= begin() && pos <= end());
		Iterator result;
		auto distance_ = std::distance(cbegin(), pos);
		++size_;
		if(size_ > capacity_){
			Resize(size_);
		}
		Iterator newPos = begin() + distance_;
		result = std::copy_backward(newPos, (begin() + (size_ - 1)), end());
		*(result - 1) = value;
		return (result - 1);
	}

	Iterator Insert(ConstIterator pos, Type&& value) {
		assert(pos >= begin() && pos <= end());
		Iterator result;
		auto distance_ = std::distance(cbegin(), pos);
		++size_;
		if(size_ > capacity_){
			Resize(size_);
		}
		Iterator newPos = begin() + distance_;
		result = std::move_backward(newPos, (begin() + (size_ - 1)), end());
		*(result - 1) = std::move(value);
		return (result - 1);
	}

	void PopBack() noexcept {
		if(size_ > 0){
			--size_;
		}
	}

	Iterator Erase(ConstIterator pos) {
		assert(pos >= begin() && pos < end());
		auto distance_ = std::distance(cbegin(), pos);
		for(size_t i = distance_; i < size_; ++i){
			*(begin() + i) = std::move(*(begin() + i + 1));
		}
		--size_;
		return (begin() + distance_);
	}

    size_t GetSize() const noexcept {
        return size_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    Type& operator[](size_t index) noexcept {
    	assert(index < size_);
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept {
    	assert(index < size_);
    	return items_[index];
    }

    const Type& At(size_t index) const{
        if(index >= size_){
        	throw std::out_of_range("out_of_range");
        }
        return items_[index];
    }

    void Clear() noexcept {
        size_ = 0;
    }

    void Resize(size_t new_size) {
		if(new_size > capacity_){
			ArrayPtr<Type> newArray(new_size * 2);
			std::move(items_.Get(), (items_.Get() + capacity_), newArray.Get());
			for(auto it = size_; it < new_size; ++it){
				newArray[it] = Type{};
			}
			items_.Swap(newArray);
			capacity_ = new_size * 2;
			size_ = new_size;
		}else{
			if(new_size > size_){
				for(auto it = size_; it < new_size; ++it){
					items_[it] = Type{};
				}
			}
			size_ = new_size;
		}
	}

    void Reserve(size_t new_capacity){
    	if(new_capacity > capacity_){
    		ArrayPtr<Type> newArray(new_capacity);
    		std::copy(items_.Get(), (items_.Get() + capacity_), newArray.Get());
    		std::fill((begin() + size_), end(), 0);
    		items_.Swap(newArray);
    		capacity_ = new_capacity;
    	}
    }

	void swap(SimpleVector& other) noexcept {
		size_t tempSize = size_;
		size_t tempCapacity = capacity_;
		size_ = other.GetSize();
		capacity_ = other.GetCapacity();
		other.size_ = tempSize;
		other.capacity_ = tempCapacity;
		items_.Swap(other.items_);
	}

    Iterator begin() noexcept {
    	return items_.Get();
    }

    Iterator end() noexcept {
    	return (items_.Get() + size_);
    }

    ConstIterator begin() const noexcept {
    	return items_.Get();
    }

    ConstIterator end() const noexcept {
    	return (items_.Get() + size_);
    }

    ConstIterator cbegin() const noexcept {
    	return items_.Get();
    }

    ConstIterator cend() const noexcept {
    	return (items_.Get() + size_);
    }
private:
    ArrayPtr<Type> items_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template <typename Type>
void swap(SimpleVector<Type>& lhs, SimpleVector<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if(lhs == rhs){
    	return false;
    }
    return true;
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if(lhs < rhs || lhs == rhs){
    	return true;
    }
    return false;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	return std::lexicographical_compare(rhs.begin(), rhs.end(), lhs.begin(), lhs.end());
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
	if(rhs < lhs || lhs == rhs){
		return true;
	}
	return false;
}

SimpleVector<int> Reserve(size_t capacity){
	SimpleVector<int> result;
	result.Reserve(capacity);
	return result;
}
