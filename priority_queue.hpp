#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

//    compare()(
/**
 * a container like std::priority_queue which is a heap internal.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
public:

//    Compare()(x,y); x<y:true

    class node {
    public:
        T _val;
        node *_lson, *_rson;
        int dis;
        size_t size;
        explicit node(const T &val, size_t sz = 1, int dist = 0, node *lson = nullptr, node *rson = nullptr):
                _val(val), _lson(lson), _rson(rson), dis(dist), size(sz){}
    };

    size_t get_size(node* ptr) const{
        if (ptr == nullptr) return 0;
        return ptr->size;
    }
    int get_dis(node* ptr) const{
        if (ptr == nullptr) return -1;
        return ptr->dis;
    }

    node* copy_all(node* ptr){
        if (ptr == nullptr) return nullptr;
        node* new_node = new node(ptr->_val);
        new_node->dis = ptr->dis;
        new_node->size = ptr->size;
        new_node->_lson = copy_all(ptr->_lson);
        new_node->_rson = copy_all(ptr->_rson);
        return new_node;
    }
    void dfs_release(node* ptr){
        if (ptr == nullptr) return;
        dfs_release(ptr->_lson);
        dfs_release(ptr->_rson);
        delete ptr;
    }
    void set_empty(){
        dfs_release(self);
        self = nullptr;
    }

	node* self;

	priority_queue() : self(nullptr){}
    explicit priority_queue(const T& val){
        self = new node(val);
        self->size = 1;
    }
	priority_queue(const priority_queue &other){
        self = copy_all(other.self);
    }
	~priority_queue() {
        set_empty();
    }

	priority_queue &operator=(const priority_queue &other) {
        if (this == &other) return *this;
        set_empty();
        self = copy_all(other.self);
        return *this;
    }
	const T & top() const {
        if (empty()) throw sjtu::container_is_empty();
        return self->_val;
    }

	void push(const T &e) {
        node* new_node = new node(e);
        if (self == nullptr) {
            self = new_node; return;
        }
        try{
            if (Compare()(self->_val, e))  std::swap(self, new_node);
            merge_node(self, new_node);
        }
        catch(...){
            delete new_node;
        }
//        std::cout<<self->size<<','<<new_node->size<<';';
	}
	void pop() {
        if (empty()) throw sjtu::container_is_empty();
        node *tmp = self->_lson, *tmp2 = self->_rson;
        if (tmp2 != nullptr){
            if ( Compare()(tmp->_val, tmp2->_val) ) std::swap(tmp, tmp2);
            merge_node(tmp, tmp2);
        }
        delete self;
        self = tmp;
	}

	size_t size() const {
        return get_size(self);
    }
	bool empty() const {
        return !get_size(self);
	}


    void update(node* ptr){
        ptr->size = get_size(ptr->_lson) + get_size(ptr->_rson) + 1;
        if (get_dis(ptr->_rson) > get_dis(ptr->_lson))
            std::swap(ptr->_rson, ptr->_lson);
        ptr->dis = get_dis(ptr->_rson) + 1;
    }
    bool merge_node(node* ptr1, node* ptr2){
        if (ptr2 == nullptr) return false;
        if (ptr1->_rson == nullptr) {
            ptr1->_rson = ptr2;
            update(ptr1);
            return false;
        }
        try {
            bool correct = Compare()(ptr2->_val, ptr1->_rson->_val);

            if (correct){
               bool ret = merge_node(ptr1->_rson, ptr2);
               if (ret) return true;
            } else {
                node* tmp = ptr1->_rson;
                ptr1->_rson = ptr2;
                bool ret = merge_node(ptr2, tmp);
                if (ret){
                    ptr1->_rson = tmp;
                    return true;
                }
            }
            update(ptr1);
        } catch(...){
            return true;
        }
        return false;
    }

	void merge(priority_queue &other) {
        if (this == &other) return;

        node *ptr1 = self, *ptr2 = other.self;
        if (ptr1 == nullptr) std::swap(ptr1, ptr2);
        if (ptr2 != nullptr) {
            try{
                if (Compare()(ptr1->_val, ptr2->_val)) std::swap(ptr1, ptr2);
            } catch(...){
                return;
            }
            bool ret = merge_node(ptr1, ptr2);
            if (ret) return;
        }
        self = ptr1;
        other.self = nullptr;
	}
};

}

#endif
