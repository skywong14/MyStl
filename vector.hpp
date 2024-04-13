#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <memory>

namespace sjtu
{
template<typename T>
class vector
{
public:
	class const_iterator;
	class iterator{
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;
	public:
        T* ptr_;
        int pos_;
	public:
        explicit iterator(T* _ptr = nullptr, int _pos = 0) : ptr_(_ptr), pos_(_pos) {}
        iterator(const iterator& other) : ptr_(other.ptr_), pos_(other.pos_) {}
        T* ptr(){
            return ptr_ + pos_;
        }
		iterator operator+(const int &n) const{
            return iterator(ptr_, pos_ + n);
		}
		iterator operator-(const int &n) const{
			return iterator(ptr_, pos_ - n);
		}
		int operator-(const iterator &rhs) const{
            if ( ptr_ != rhs.ptr_ ) throw invalid_iterator();
            return pos_ - rhs.pos_;
        }
		iterator& operator+=(const int &n){
            pos_ = pos_ + n;
            return (*this);
		}
		iterator& operator-=(const int &n){
            pos_ = pos_ - n;
            return (*this);
		}
		iterator operator++(int) {
            iterator tmp(*this);
            pos_++;
            return tmp;
        }
		iterator& operator++() {
            pos_++;
            return (*this);
        }
		iterator operator--(int){
            iterator tmp(*this);
            pos_--;
            return tmp;
        }
		iterator& operator--(){
            pos_--;
            return (*this);
        }
		T& operator*() const{
            return *(ptr_ + pos_);
        }
		bool operator==(const iterator &rhs) const{ return (ptr_ == rhs.ptr_) && (pos_ == rhs.pos_); }
		bool operator==(const const_iterator &rhs) const{ return (ptr_ == rhs.ptr_) && (pos_ == rhs.pos_); }
		bool operator!=(const iterator &rhs) const{ return (ptr_ != rhs.ptr_) || (pos_ != rhs.pos_); }
		bool operator!=(const const_iterator &rhs) const{ return (ptr_ != rhs.ptr_) || (pos_ != rhs.pos_); }
	};
	class const_iterator {
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = T*;
		using reference = T&;
		using iterator_category = std::output_iterator_tag;
    public:
        T* ptr_;
        int pos_;
    public:
        explicit const_iterator(T* _ptr = nullptr, int _pos = 0) : ptr_(_ptr), pos_(_pos) {}
        const_iterator(const iterator& other) : ptr_(other.ptr_), pos_(other.pos_) {}
        const_iterator(const const_iterator& other) : ptr_(other.ptr_), pos_(other.pos_) {}
        T* ptr(){
            return ptr_ + pos_;
        }
        const_iterator operator+(const int &n) const{
            return const_iterator(ptr_, pos_ + n);
        }
        const_iterator operator-(const int &n) const{
            return const_iterator(ptr_, pos_ - n);
        }
        const_iterator& operator+=(const int &n){
            pos_ = pos_ + n;
            return (*this);
        }
        const_iterator& operator-=(const int &n){
            pos_ = pos_ - n;
            return (*this);
        }
        const_iterator operator++(int) {
            const_iterator tmp(*this);
            pos_++;
            return tmp;
        }
        const_iterator& operator++() {
            pos_++;
            return (*this);
        }
        const_iterator operator--(int){
            const_iterator tmp(*this);
            pos_--;
            return tmp;
        }
        int operator-(const const_iterator &rhs) const{
            if ( ptr_ != rhs.ptr_ ) throw invalid_iterator();
            return pos_ - rhs.pos_;
        }
        const T& operator*() const{
            return *(ptr_ + pos_);
        }
        bool operator==(const iterator &rhs) const{ return (ptr_ == rhs.ptr_) && (pos_ == rhs.pos_); }
        bool operator==(const const_iterator &rhs) const{ return (ptr_ == rhs.ptr_) && (pos_ == rhs.pos_); }
        bool operator!=(const iterator &rhs) const{ return (ptr_ != rhs.ptr_) || (pos_ != rhs.pos_); }
        bool operator!=(const const_iterator &rhs) const{ return (ptr_ != rhs.ptr_) || (pos_ != rhs.pos_); }
	};

    iterator beg_, end_, limit_;

    void deallocate_memory(){
        std::allocator<T>().deallocate(beg_.ptr(), limit_ - beg_);
    }

	vector(): beg_(nullptr), end_(nullptr), limit_(nullptr){}
    void assign_init_value(const vector &other){
        size_t max_size = other.max_size(), new_size = other.size();
        beg_ = iterator(std::allocator<T>().allocate(max_size), 0);
        end_ = beg_ + new_size;
        limit_ = beg_ + max_size;
        iterator it = other.beg_;
        T* ptr = beg_.ptr();
        for (int i = 0; i < new_size; i++, it++, ptr++){
            std::construct_at(ptr, *it);
        }
    }
	vector(const vector &other) {
        assign_init_value(other);
    }
	vector &operator=(const vector &other) {
        if (this == &other) return (*this);
        clear();
        assign_init_value(other);
        return *this;
    }
    ~vector() {
        std::destroy(beg_.ptr(), end_.ptr());
        deallocate_memory();
    }
	T & at(const size_t &pos){
        if (pos >= size()) throw index_out_of_bound();
        return *(beg_.ptr_ + pos);
    }

	const T & at(const size_t &pos) const{
        if (pos >= size()) throw index_out_of_bound();
        return *(beg_.ptr_ + pos);
    }

	T & operator[](const size_t &pos) {
        if (pos < 0 || pos >= size()) throw index_out_of_bound();
        return *(beg_.ptr_ + pos);
    }
	const T & operator[](const size_t &pos) const {
        if (pos < 0 || pos >= size()) throw index_out_of_bound();
        return *(beg_.ptr_ + pos);
    }
	const T & front() const {
        if (empty()) throw container_is_empty();
        return *beg_;
    }
	const T & back() const {
        if (empty()) throw container_is_empty();
        return *(end_ - 1);
    }

	iterator begin() { return beg_; }
	const_iterator cbegin() const { return const_iterator(beg_); }
	iterator end() { return end_; }
	const_iterator cend() const { return const_iterator(end_); }
	bool empty() const{ return (end_ == beg_); }
	size_t size() const{ return const_iterator(end_) - const_iterator(beg_); }
    size_t max_size() const{ return limit_ - beg_; }
	void clear(){
        std::destroy(beg_.ptr(), end_.ptr());
        deallocate_memory();
        end_ = beg_ = limit_ = iterator(nullptr, 0);
    }

    void check_half(){
        if ((end_ - beg_) * 3 < limit_ - beg_ ){
            int new_size = limit_.pos_ / 2;
            int sz = size();
            iterator new_beg = iterator(std::allocator<T>().allocate(new_size), 0);
            iterator new_end = new_beg + sz;
            iterator new_limit = new_beg + new_size;
            std::uninitialized_copy(beg_.ptr(), end_.ptr(), new_beg.ptr());
            std::destroy(beg_.ptr(), end_.ptr());

            deallocate_memory();
            beg_ = new_beg;
            end_ = new_end;
            limit_ = new_limit;
        }
    }

    iterator insert_aux(iterator pos, const T& value){
        if (end_ != limit_){
            std::construct_at(end_.ptr(), value);
            end_++;
            std::copy_backward(pos.ptr(), (end_ - 1).ptr(), end_.ptr());
            *pos = value;
            return pos;
        } else {
            size_t bef_size = size(), new_size = 0;
            if (bef_size == 0) new_size = 1;
            else new_size = bef_size * 2;
            int ind = pos.pos_;
            iterator new_beg = iterator(std::allocator<T>().allocate(new_size), 0);
            std::uninitialized_copy(beg_.ptr(), pos.ptr(), new_beg.ptr());
            iterator new_limit = new_beg + new_size;
            iterator ret_it = new_beg + ind;
            std::construct_at(ret_it.ptr(), value);
            std::uninitialized_copy(pos.ptr(), end_.ptr(), (ret_it + 1).ptr());
            iterator new_end = new_beg + bef_size + 1;

            std::destroy(beg_.ptr(), end_.ptr());
            deallocate_memory();
            beg_ = new_beg;
            end_ = new_end;
            limit_ = new_limit;
            return ret_it;
        }
    }

	iterator insert(iterator pos, const T &value) {
        return insert_aux(pos, value);
    }

	iterator insert(const size_t &ind, const T &value) {
        if (ind > size()) throw index_out_of_bound();
        if (ind == size()) {
            push_back(value);
            return end_ - 1;
        }
        iterator tmp(beg_ + ind);
        return insert_aux(tmp, value);
    }

	iterator erase(iterator pos){
        size_t ind = pos.pos_;
        std::copy_backward((pos + 1).ptr(), end_.ptr(), (end_ - 1).ptr());
        std::destroy_at(end_.ptr());
        end_--;
        check_half();
        return beg_ + ind;
    }

	iterator erase(const size_t &ind) {
        if (ind >= size()) throw index_out_of_bound();
        return erase(beg_ + ind);
    }

	void push_back(const T &value) {
        if (end_ != limit_){
            std::construct_at(end_.ptr(), value);
            end_++;
        } else
            insert_aux(end(), value);
    }
	void pop_back() {
        if (empty()) throw container_is_empty();
        --end_;
        std::destroy_at(end_.ptr());
        check_half();
    }
};

}

#endif
