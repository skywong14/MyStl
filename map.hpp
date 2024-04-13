#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>
#include <cstddef>
#include <memory>
#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    > class map {
    private:
        static const bool RED = false, BLACK = true;
    public:
        typedef pair<const Key, T> value_type;

        struct Node{
            bool color;
            value_type val;
            Node* parent;
            Node* lson;
            Node* rson;
            Node(const value_type& _val):val(_val), color(true), lson(nullptr), rson(nullptr), parent(nullptr){}
        };
        Key get_key(Node* ptr) const{ //or const Key&
            return ptr->val.first;
        }

        class const_iterator;
        class iterator {
        private:
        public:
            Node* ptr;
            Node* origin;
            iterator(): ptr(nullptr),origin(nullptr) {}
            iterator(const iterator &other) {
                ptr = other.ptr;
                origin = other.origin;
            }
            explicit iterator(Node* p, Node* origin_){ ptr = p; origin = origin_;}

            void increment(){
                Node* tmp;
                //end()
                if (ptr->parent == nullptr || ptr->parent->parent == ptr && ptr->color == RED){
                    throw invalid_iterator();
                }
                //other condition
                if (ptr->rson != nullptr){
                    //if the tree empty header's rson is header
                    tmp = ptr->rson;
                    while (tmp->lson != nullptr){
                        tmp = tmp->lson;
                    }
                    ptr = tmp;
                } else {
                    tmp = ptr;
                    while (tmp->parent != nullptr && tmp->parent->rson == tmp){
                        tmp = tmp->parent;
                    }
                    if (tmp->parent->parent == tmp && tmp->color == RED) ptr = tmp;
                    else ptr = tmp->parent;
                }
            }
            void decrement(){
                //end()
                if (ptr->parent == nullptr) throw invalid_iterator(); //tree is empty
                if (ptr->parent->parent == ptr && ptr->color == RED){
                    ptr = ptr->rson;
                    return;
                }
                //normal
                Node* tmp;
                if (ptr->lson != nullptr){
                    tmp = ptr->lson;
                    while (tmp->parent != nullptr && tmp->rson != nullptr){
                        tmp = tmp->rson;
                    }
                    ptr = tmp;
                } else {
                    tmp = ptr;
                    while (tmp->parent->lson == tmp){
                        tmp = tmp->parent;
                    }
                    if (tmp->parent->parent == tmp && tmp->color == BLACK && tmp->parent->lson == ptr) throw invalid_iterator();
                    ptr = tmp->parent;
                }
            }

            iterator operator++(int) {
                iterator tmp = *this;
                increment();
                return tmp;
            }
            iterator & operator++() {
                increment();
                return *this;
            }
            iterator operator--(int) {
                iterator tmp = *this;
                decrement();
                return tmp;
            }
            iterator & operator--() {
                decrement();
                return *this;
            }
            value_type & operator*() const {
                return ptr->val;
            }

            value_type* operator->() const noexcept { return &(operator*()); }

            bool operator==(const iterator &rhs) const { return (ptr == rhs.ptr); }
            bool operator==(const const_iterator &rhs) const { return (ptr == rhs.ptr); }
            bool operator!=(const iterator &rhs) const { return (ptr != rhs.ptr); }
            bool operator!=(const const_iterator &rhs) const { return (ptr != rhs.ptr); }
        };
        class const_iterator {
        private:
        public:
            Node* ptr;
            Node* origin;
            const_iterator() : ptr(nullptr), origin(nullptr) {}
            const_iterator(const const_iterator &other) {
                ptr = other.ptr;
                origin = other.origin;
            }
            const_iterator(const iterator &other) {
                ptr = other.ptr;
                origin = other.origin;
            }
            explicit const_iterator(Node* p, Node* origin_){ ptr = p; origin = origin_;}

            void increment(){
                Node* tmp;
                //end()
                if (ptr->parent == nullptr || ptr->parent->parent == ptr && ptr->color == RED){
                    throw invalid_iterator();
                }
                //other condition
                if (ptr->rson != nullptr){
                    //if the tree empty header's rson is header
                    tmp = ptr->rson;
                    while (tmp->lson != nullptr){
                        tmp = tmp->lson;
                    }
                    ptr = tmp;
                } else {
                    tmp = ptr;
                    while (tmp->parent->rson == tmp){
                        tmp = tmp->parent;
                    }
                    if (tmp->parent->parent == tmp && tmp->color == RED) ptr = tmp;
                    else ptr = tmp->parent;
                }
            }
            void decrement(){
                //end()
                if (ptr->parent == nullptr) throw invalid_iterator(); //tree is empty
                if (ptr->parent->parent == ptr && ptr->color == RED){
                    ptr = ptr->rson;
                    return;
                }
                //normal
                Node* tmp;
                if (ptr->lson != nullptr){
                    tmp = ptr->lson;
                    while (tmp->rson != nullptr){
                        tmp = tmp->rson;
                    }
                    ptr = tmp;
                } else {
                    tmp = ptr;
                    while (tmp->parent->lson == tmp){
                        tmp = tmp->parent;
                    }
                    if (tmp->parent->parent == tmp && tmp->color == BLACK && ptr == tmp->parent->lson) throw invalid_iterator();
                    ptr = tmp->parent;
                }
            }

            const_iterator operator++(int){
                const_iterator tmp = *this;
                increment();
                return tmp;
            }
            const_iterator & operator++(){
                increment();
                return *this;
            }
            const_iterator operator--(int){
                const_iterator tmp = *this;
                decrement();
                return tmp;
            }
            const_iterator & operator--(){
                decrement();
                return *this;
            }
            const value_type& operator*() const {
                return ptr->val;
            }
            const value_type* operator->() const noexcept { return &(operator*()); }

            bool operator==(const iterator &rhs) const { return (ptr == rhs.ptr); }
            bool operator==(const const_iterator &rhs) const { return (ptr == rhs.ptr); }
            bool operator!=(const iterator &rhs) const { return (ptr != rhs.ptr); }
            bool operator!=(const const_iterator &rhs) const { return (ptr != rhs.ptr); }
        };

        //node
        Node* leftist_node(Node* ptr){ if (ptr == nullptr) return ptr; while (ptr->lson != nullptr && ptr->lson != ptr) ptr = ptr->lson; return ptr; }
        Node* rightist_node(Node* ptr){ if (ptr == nullptr) return ptr; while (ptr->rson != nullptr && ptr->rson != ptr) ptr = ptr->rson; return ptr; }

        Node* allocate_node(){ return std::allocator<Node>().allocate(1); }
        void deallocate_node(Node* ptr){ std::allocator<Node>().deallocate(ptr, 1); }
        Node* create_node(const value_type& _val){
            Node* new_node = allocate_node();
            std::construct_at(new_node, _val);
            return new_node;
        }
        Node* copy_node(Node* ptr){
            Node* new_ptr = create_node(ptr->val);
            new_ptr->color = ptr->color; new_ptr->lson = 0; new_ptr->rson = 0;
            return new_ptr;
        }
        void destroy_node(Node* ptr){
            std::destroy_at(ptr);
            deallocate_node(ptr);
        }

        //header, node_count
        size_t node_num;
        Node* header;
        Node*& root() const { return (Node*&)header->parent; }
        Node*& header_left() const { return (Node*&)header->lson; }
        Node*& header_right() const { return (Node*&)header->rson; }


        void init_map(){
            header = allocate_node();
            header->color = RED;
            root() = nullptr;
            header_left() = header_right() = header;
        } //配置header
        Node* copy_all(Node* other){
            if (other == nullptr) return nullptr;
            Node* new_node = copy_node(other);
            if (other->lson != nullptr) {
                new_node->lson = copy_all(other->lson);
                new_node->lson->parent = new_node;
            }
            if (other->rson != nullptr) {
                new_node->rson = copy_all(other->rson);
                new_node->rson->parent = new_node;
            }
            return new_node;
        }

        map():node_num(0) { init_map(); }
        map(const map &other) {
            init_map();
            node_num = other.node_num;
            root() = copy_all(other.root());
            if (root() != nullptr) root()->parent = header;
            header->lson = leftist_node(root());
            header->rson = rightist_node(root());
            if (header->lson == nullptr) header->lson = header;
            if (header->rson == nullptr) header->rson = header;
        }
        map & operator=(const map &other) {
            if (this == &other) return *this;
            clear();
            node_num = other.node_num;
            root() = copy_all(other.root());
            if (root() != nullptr) root()->parent = header;
            header->lson = leftist_node(root());
            header->rson = rightist_node(root());
            if (header->lson == nullptr) header->lson = header;
            if (header->rson == nullptr) header->rson = header;
            return *this;
        }

        ~map() {
            clear();
            deallocate_node(header);
        }

        void L(Node* ptr){
            Node* tmp = ptr->rson;
            ptr->rson = tmp->lson;
            tmp->lson = ptr;
            if (ptr->rson != nullptr) ptr->rson->parent = ptr;
            tmp->parent = ptr->parent;

            if (ptr == root()) {
                root() = tmp;
            }
            else {
                if (ptr == ptr->parent->lson) ptr->parent->lson = tmp;
                else ptr->parent->rson = tmp;
            }

            ptr->parent = tmp;
        }
        void R(Node* ptr){
            Node* tmp = ptr->lson;
            ptr->lson = tmp->rson;
            tmp->rson = ptr;
            if (ptr->lson != nullptr) ptr->lson->parent = ptr;
            tmp->parent = ptr->parent;

            if (ptr == root()) root() = tmp;
            else {
                if (ptr == ptr->parent->rson) ptr->parent->rson = tmp;
                else ptr->parent->lson = tmp;
            }

            ptr->parent = tmp;
        }

        void rebalance_tree(Node* x){
            x->color = RED;
            Node* uncle;
            bool uncle_col;
            while (x != root() && x->parent->color == RED){
                if (x->parent == x->parent->parent->lson){
                    uncle = x->parent->parent->rson;
                    if (uncle == nullptr) uncle_col = BLACK;
                    else uncle_col = uncle->color;
                    //case 1:
                    //            B                      x(R)
                    //         R     R        ->        B   B
                    //     x(R) .   .  .               R .  . .
                    if (uncle_col == RED){
                        x->parent->color = BLACK;
                        if (uncle != nullptr) uncle->color = BLACK;
                        x->parent->parent->color = RED;
                        x = x->parent->parent;
                    }
                    //case 2:
                    //            B                       R                B
                    //         R     B        ->        B   B    ->     R     R
                    //     x(R) .   .  .               R 1  2 3              1  B
                    //                                                         2 3
                    if (uncle_col == BLACK){
                        if (x == x->parent->rson){
                            //            R                      x(R)             R
                            //         B     x(R)       ->     R    4   ->   x(R)     4
                            //        1 2   3 4               B  3          B   3
                            x = x->parent;
                            L(x); //注意uncle可能不存在
                        }
                        if (uncle != nullptr) x->parent->color = BLACK;
                        x->parent->parent->color = RED;
                        x->parent->color = BLACK;
                        R(x->parent->parent);
                        break;
                    }
                } else {
                    uncle = x->parent->parent->lson;
                    if (uncle == nullptr) uncle_col = BLACK;
                    else uncle_col = uncle->color;
                    //case 3:
                    //            B                      x(R)
                    //         R     R        ->        B   B
                    //        . .   . x(R)             . .  . R
                    if (uncle_col == RED){
                        x->parent->color = BLACK;
                        if (uncle != nullptr) uncle->color = BLACK;
                        x->parent->parent->color = RED;
                        x = x->parent->parent;
                    }
                    //case 4
                    if (uncle_col == BLACK){
                        if (x == x->parent->lson){
                            x = x->parent;
                            R(x);
                        }
                        x->parent->color = BLACK;
                        x->parent->parent->color = RED;
                        L(x->parent->parent);
                        break;
                    }
                }

            }
            root()->color = BLACK;
        }

        //新节点，新节点父亲，插入值
        iterator _insert(Node* new_node_, Node* parent_node_, const value_type& val_){
            Node* new_node;
            if (parent_node_ == header || Compare()(val_.first, get_key(parent_node_))){
                //Compare须在最后
                //insert on left
                new_node = create_node(val_);
                parent_node_->lson = new_node;
                if (parent_node_ == header){
                    //该节点为root
                    root() = new_node;
                    header_left() = new_node;
                    header_right() = new_node;
                } else if (parent_node_ == header_left()){
                    //比原来最小值更小
                    header_left() = new_node;
                }
            } else{
                //insert on right
                new_node = create_node(val_);
                parent_node_->rson = new_node;
                if (parent_node_ == header_right()){
                    //比原来最大值更大
                    header_right() = new_node;
                }
            }
            new_node->rson = new_node->lson = nullptr;
            new_node->parent = parent_node_;

            rebalance_tree(new_node);
            node_num++;
            return iterator(new_node, header);
        }
        pair<iterator, bool> insert_unique(const value_type& val){
            //find the position to insert
            Node* pre = header, *cur = root();
            bool equal = false;
            while (cur != nullptr){
                pre = cur;
                if (Compare()(val.first, get_key(cur))){ //val < cur, go left
                    cur = cur->lson;
                } else if (Compare()(get_key(cur), val.first)){ // cur < val
                    cur = cur->rson;
                } else {
                    equal = true; break;
                }
            }

            //insert
            iterator now_it = iterator( pre , header);

            //if equal:
            if (equal) return pair<iterator, bool>(now_it, false);

            //not equal
            return pair<iterator, bool>(_insert( cur, pre, val ), true);
        }

        T & at(const Key &key) {
            iterator it = find(key);
            if (it == end()) throw index_out_of_bound();
            return (*it).second;
        }
        const T & at(const Key &key) const {
            const_iterator it = find(key);
            if (it == cend()) throw index_out_of_bound();
            return (*it).second;
        }

        T & operator[](const Key &key) {
            iterator it = insert(pair<const Key, T>(key, T())).first;
            return (*it).second;
        }

        const T & operator[](const Key &key) const {
            const_iterator it = find(key);
            if (it == cend()) throw index_out_of_bound();
            return (*it).second;
        }

        iterator begin() { return iterator(header_left(), header); }
        const_iterator cbegin() const { return const_iterator(header_left(), header); }

        iterator end() { return iterator(header, header); }
        const_iterator cend() const { return const_iterator(header, header); }

        bool empty() const { return !node_num; }
        size_t size() const { return node_num; }


        void clear_dfs(Node* ptr){
            if (ptr->lson != nullptr) clear_dfs(ptr->lson);
            if (ptr->rson != nullptr) clear_dfs(ptr->rson);
            destroy_node(ptr);
        }
        void clear() {
            if (root() != nullptr) clear_dfs(root());
            node_num = 0;
            root() = nullptr;
            header->lson = header->rson = header;
        }

        pair<iterator, bool> insert(const value_type &value) {
            return insert_unique(value);
        }


        Node* get_Sibling(Node* ptr){
            if (ptr == ptr->parent->lson) return ptr->parent->rson;
            return ptr->parent->lson;
        }
        bool get_color(Node* ptr) const{
            if (ptr != nullptr) return ptr->color;
            return BLACK;
        }

        //x为根的子树“黑色高度”偏小1（或将要偏小1），故需调整
        //x应当是黑色，且有Sibling node
        void rebalance_after_erase(Node* x){
            if (x == root()) return; // no need for root to balance
            Node* sib = get_Sibling(x);

            //case 1:sib.color == red
            if (sib->color == RED){
                //       B(P)                  R                 B
                //      / \     L_rotate      / \               / \
            //    B(x) R                B(P)  2          R(P)   2
                //        / \               / \               / \
            //       1   2            x    1            x    1
                Node* fa = x->parent;
                if (x == x->parent->lson)
                    L(fa);
                else R(fa);
                sib->color = BLACK;
                fa->color = RED;
                sib = get_Sibling(x);
                //continue to other case
            }
            //sib.color already be black

            //close_nephew and distant_nephew
            Node* close_nephew, *distant_nephew;
            if (x->parent->lson == x){
                close_nephew = x->parent->rson->lson;
                distant_nephew = x->parent->rson->rson;
            } else {
                close_nephew = x->parent->lson->rson;
                distant_nephew = x->parent->lson->lson;
            }

            //case 2
            if ((get_color(close_nephew) == BLACK) && (get_color(distant_nephew) == BLACK)
                && (get_color(x->parent) == RED)){
                //       R(P)                  B(P)             B
                //      / \                   / \               / \
            //    B(x) B               B(x)  R          B(P)   R
                //        / \                   / \               / \
            //     B(1) B(2)              B(1) B(2)         B(1) B(2)
                sib->color = RED;
                x->parent->color = BLACK;
                return;
            }
            //case 3
            if ((get_color(close_nephew) == BLACK) && (get_color(distant_nephew) == BLACK)
                && (get_color(x->parent) == BLACK)){
                //       B(P)                  B(P)
                //      / \                   / \
            //    B(x) B               B(x)  R
                //        / \                   / \
            //     B(1) B(2)              B(1) B(2)
                sib->color = RED;
                rebalance_after_erase(x->parent); //现在P节点的黑色高度变小1
                return;
            }
            //case 4
            if (get_color(close_nephew) == RED && get_color(distant_nephew) == BLACK){
                //       ?(P)                   ?(P)                  ?(P)
                //      / \                    / \                  /   \
            //    B(x) B               B(x)  R(1)             B(x)    B(1)
                //        / \      R(Sib)         / \                    / \
            //     R(1) B(2)              B(3)   B                B(3)   R
                //     / \                          / \                     / \
            //  B(3) B(4)                    B(4)  B(2)              B(4)  B(2)
                if (x == x->parent->lson){
                    R(sib);
                } else {
                    L(sib);
                }
                sib->color = RED;
                close_nephew->color = BLACK;
                //update sib and nephew
                sib = get_Sibling(x);
                if (x->parent->lson == x){
                    close_nephew = x->parent->rson->lson;
                    distant_nephew = x->parent->rson->rson;
                } else {
                    close_nephew = x->parent->lson->rson;
                    distant_nephew = x->parent->lson->lson;
                }
                //continue to case 5
            }
            if (get_color(sib) == BLACK && get_color(distant_nephew) == RED){
                //       ?(P)                   B                    ?
                //      / \                    / \                  /   \
            //    B(x) B       L(P)      ?(P)  R(2)          B(P)    B(2)
                //        / \               / \                  / \
            //     ?(1) R(2)         B(x) B(1)             B(x) ?(1)
                if (x == x->parent->lson)
                    L(x->parent);
                else
                    R(x->parent);
                bool tmp_col = x->parent->color;
                x->parent->parent->color = tmp_col;
                x->parent->color = BLACK;
                if (x->parent == x->parent->parent->lson){
                    if (x->parent->parent->rson != nullptr)
                        x->parent->parent->rson->color = BLACK;
                }
                else{
                    if (x->parent->parent->lson != nullptr)
                        x->parent->parent->lson->color = BLACK;
                }
                return;
            }
        }

        void swap_node(Node* ptr1, Node* ptr2){
            Node* t1;
            bool t2;
            if (ptr1->parent == ptr2){
                int flag = 0;
                if (ptr2 == root()) flag = 1;

                if (ptr2 == ptr2->parent->lson) ptr2->parent->lson = ptr1;
                else ptr2->parent->rson = ptr1;
                if (ptr1->lson != nullptr) ptr1->lson->parent = ptr2;
                if (ptr1->rson != nullptr) ptr1->rson->parent = ptr2;

                ptr1->parent = ptr2->parent; ptr2->parent = ptr1;
                t2 = ptr2->color; ptr2->color = ptr1->color; ptr1->color = t2;

                if (ptr1 == ptr2->lson) {
                    ptr2->rson->parent = ptr1;
                    ptr2->lson = ptr1->lson;
                    ptr1->lson = ptr2;
                    t1 = ptr2->rson; ptr2->rson = ptr1->rson; ptr1->rson = t1;
                } else {
                    ptr2->lson->parent = ptr1;
                    ptr2->rson = ptr1->rson;
                    ptr1->rson = ptr2;
                    t1 = ptr2->lson; ptr2->lson = ptr1->lson; ptr1->lson = t1;
                }

                if (flag) {
                    root() = ptr1;
                    header->lson = leftist_node(root());
                    header->rson = rightist_node(root());
                }
            } else if (ptr2->parent == ptr1){
                int flag = 0;
                if (ptr1 == root()) flag = 1;

                if (ptr1 == ptr1->parent->lson) ptr1->parent->lson = ptr2;
                else ptr1->parent->rson = ptr2;
                if (ptr2->lson != nullptr) ptr2->lson->parent = ptr1;
                if (ptr2->rson != nullptr) ptr2->rson->parent = ptr1;

                ptr2->parent = ptr1->parent; ptr1->parent = ptr2;
                t2 = ptr1->color; ptr1->color = ptr2->color; ptr2->color = t2;

                if (ptr2 == ptr1->lson) {
                    ptr1->rson->parent = ptr2;
                    ptr1->lson = ptr2->lson;
                    ptr2->lson = ptr1;
                    t1 = ptr1->rson; ptr1->rson = ptr2->rson; ptr2->rson = t1;
                } else {
                    ptr1->lson->parent = ptr2;
                    ptr1->rson = ptr2->rson;
                    ptr2->rson = ptr1;
                    t1 = ptr1->lson; ptr1->lson = ptr2->lson; ptr2->lson = t1;
                }

                if (flag) {
                    root() = ptr2;
                    header->lson = leftist_node(root());
                    header->rson = rightist_node(root());
                }

            } else{
                int flag1 = 0, flag2 = 0;
                if (ptr1 == root()) flag1 = 1;
                if (ptr2 == root()) flag2 = 1;

                if (ptr1 == ptr1->parent->lson) ptr1->parent->lson = ptr2;
                else ptr1->parent->rson = ptr2;
                if (ptr2 == ptr2->parent->lson) ptr2->parent->lson = ptr1;
                else ptr2->parent->rson = ptr1;

                if (ptr1->lson != nullptr) ptr1->lson->parent = ptr2;
                if (ptr1->rson != nullptr) ptr1->rson->parent = ptr2;
                if (ptr2->lson != nullptr) ptr2->lson->parent = ptr1;
                if (ptr2->rson != nullptr) ptr2->rson->parent = ptr1;

                t1 = ptr1->lson; ptr1->lson = ptr2->lson; ptr2->lson = t1;
                t1 = ptr1->rson; ptr1->rson = ptr2->rson; ptr2->rson = t1;
                t1 = ptr1->parent; ptr1->parent = ptr2->parent; ptr2->parent= t1;
                t2 = ptr1->color; ptr1->color = ptr2->color; ptr2->color = t2;

                if (flag1){
                    root() = ptr2;
                    header->lson = leftist_node(root());
                    header->rson = rightist_node(root());
                }
                if (flag2) {
                    root() = ptr1;
                    header->lson = leftist_node(root());
                    header->rson = rightist_node(root());
                }
            }
        }

        void erase_node(Node* x){
            //case 0: the tree size == 1;
            node_num--;
            if (node_num == 0){
                header->rson = header->lson = header;
                header->parent = nullptr;
                destroy_node(x);
                return;
            }
            //case 1
            if (x->lson != nullptr && x->rson != nullptr){
                Node* sub_node = leftist_node(x->rson);
                swap_node(x, sub_node); // then we are going to delete node_x
                //goto case 2/3
            }
            //case 2:x is leaf node
            if (x->lson == nullptr && x->rson == nullptr){
                //destroy x and maintain balance
                if (x->color == BLACK){
                    rebalance_after_erase(x);
                }

                if (x == x->parent->lson)
                    x->parent->lson = nullptr;
                else
                    x->parent->rson = nullptr;
                destroy_node(x);
                return;
            }
            //case 3: x only has one son
            Node* son;
            if (x->lson != nullptr) son = x->lson; else son = x->rson;
            son->parent = x->parent;

            if (x == root()){
                header->parent = son;
            } else {
                if (x == x->parent->lson) x->parent->lson = son;
                else x->parent->rson = son;
            }

            if (x->color == BLACK){
                if (son->color == RED){
                    son->color = BLACK;
                } else {
                    rebalance_after_erase(son);
                }
            }
            destroy_node(x);
        }

        void erase(iterator pos) {
            if (pos == cend() || pos.ptr == nullptr || pos.origin != header) throw exception();
            Node* ptr = pos.ptr;
            int tag1 = 0, tag2 = 0;
            if (ptr == header->lson) tag1 = 1;
            if (ptr == header->rson) tag2 = 1;
            erase_node(pos.ptr);
            if (tag1 && node_num)
                header->lson = leftist_node(root());
            if (tag2 && node_num)
                header->rson = rightist_node(root());
        }

        size_t count(const Key &key) const {
            if (find(key) != cend()) return 1;
            return 0;
        }
        iterator find(const Key &key) {
            Node* pre = header, *cur = root();
            bool flag = false;
            while (cur != nullptr){
                pre = cur;
                if (Compare()(key, get_key(cur) )){ //val < cur, go left
                    cur = cur->lson;
                } else if (Compare()(get_key(cur), key)){
                    cur = cur->rson;
                } else {
                    flag = true;
                    break;
                }
            }
            iterator it(pre, header);
            if (flag) return it;
            else return end();
        }
        const_iterator find(const Key &key) const {
            Node *pre = header, *cur = root();
            bool flag = false;
            while (cur != nullptr){
                pre = cur;
                if (Compare()(key, get_key(cur) )){ //val < cur, go left
                    cur = cur->lson;
                } else if (Compare()(get_key(cur), key)){
                    cur = cur->rson;
                } else {
                    flag = true;
                    break;
                }
            }
            const_iterator it(pre, header);
            if (flag) return it;
            else return cend();
        }

        int print_dfs(Node* ptr,int num){
            if (ptr == nullptr) return 0;
            int t = 1;
            for (int i = 1;i<=num;i++) std::cout<<' ';
            std::cout<<"Key:"<<ptr->val.first<<"  Color: "<<(((ptr->color)==BLACK)?"Black":"Red")<<std::endl;
            for (int i = 1;i<=num;i++) std::cout<<' ';
            std::cout<<"left:"<<std::endl;
            t += print_dfs(ptr->lson, num+3);
            for (int i = 1;i<=num;i++) std::cout<<' ';
            std::cout<<"right:"<<std::endl;
            t += print_dfs(ptr->rson, num+3);
            return t;
        }

        int print_all_tree(){
            std::cout<<"======all the tree====="<<std::endl;
            std::cout<<"root:"<<root()<<std::endl;
            std::cout<<"header:"<<header<<"    's parent:"<<header->parent->val.first<<std::endl;
            std::cout<<"      |->>> left:"<<header->lson->val.first<<"  right:"<<header->rson->val.first<<std::endl;
            int t = print_dfs(root(), 0);
            std::cout<<"==================="<<std::endl;
            return t;
        }

        value_type spe(){
            return rightist_node(root())->val;
        }
    };

}

#endif