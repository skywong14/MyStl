#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include <cstddef>
#include <memory>

namespace sjtu {

template <typename T> class list {
protected:
  class listnode {
  public:
    T _val;
    listnode *_pre, *_nxt;
    explicit listnode(const T &val, listnode *pre = nullptr, listnode *nxt = nullptr):
        _val(val), _pre(pre), _nxt(nxt){}
  };

public:
  class const_iterator;
  class iterator {
  public:
    listnode* node;
    iterator() { node = nullptr; }
    explicit iterator(listnode* p){ node = p;}
    iterator operator++(int) {
        iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    iterator &operator++() {
        node = node->_nxt;
        return *this;
    }
    iterator operator--(int) {
        iterator tmp = *this;
        --(*this);
        return tmp;
    }
    iterator &operator--() {
        node = node->_pre;
        return *this;
    }

    T &operator*() const { return node->_val; }

    T *operator->() const noexcept { return &(operator*()); }

    bool operator==(const iterator &rhs) const {
        return node == rhs.node;
    }
    bool operator==(const const_iterator &rhs) const {
        return node == rhs.node;
    }
    bool operator!=(const iterator &rhs) const {
        return !(node == rhs.node);
    }
    bool operator!=(const const_iterator &rhs) const {
        return !(node == rhs.node);
    }
  };

  class const_iterator {
  public:
      listnode* node;

      const_iterator() { node = nullptr; }
      explicit const_iterator(listnode* p){ node = p;}
      const_iterator(iterator it){
          node = it.node;
      }
      const_iterator operator++(int) {
          const_iterator tmp = *this;
          ++(*this);
          return tmp;
      }
      const_iterator &operator++() {
          node = node->_nxt;
          return *this;
      }
      const_iterator operator--(int) {
          const_iterator tmp = *this;
          --(*this);
          return tmp;
      }
      const_iterator &operator--() {
          node = node->_pre;
          return *this;
      }

      T &operator*() const { return node->_val; }

      T *operator->() const noexcept { return &(operator*()); }

      bool operator==(const iterator &rhs) const {
          return node == rhs.node;
      }
      bool operator==(const const_iterator &rhs) const {
          return node == rhs.node;
      }
      bool operator!=(const iterator &rhs) const {
          return !(node == rhs.node);
      }
      bool operator!=(const const_iterator &rhs) const {
          return !(node == rhs.node);
      }
  };


  listnode* empty_node(){
      return std::allocator<listnode>().allocate(1);
  }//配置一个空节点
  void release_node(listnode* ptr){
      std::allocator<listnode>().deallocate(ptr, 1);

  }//释放节点
  listnode* create_node(const T &val){
      listnode* p = empty_node();
      std::allocator<listnode>().construct(p, val); //for cpp11&17
//      std::construct_at(p, val); //for cpp20
      return p;
  }
  void destroy_node(listnode* p){
//      std::allocator<listnode>().destroy(&p->_val);
      std::destroy_at(&p->_val);
      release_node(p);
  }


  listnode* node;
  void empty_init(){
      node = empty_node();
      node->_pre = node;
      node->_nxt = node;
  }
  list() {
      empty_init();
  }
  list(const list &other) {
      empty_init();
      for (const_iterator it = other.cbegin(); it != other.cend(); it++){
          push_back(*it);
      }
  }
  virtual ~list() {
      clear();
      release_node(node);
  }
  list &operator=(const list &other) {
       if (&other == (this)) return *this;
      clear();
      for (const_iterator it = other.cbegin(); it != other.cend(); it++){
          push_back(*it);
      }
      return *this;
  }

  const T &front() const {
      if (empty()) throw std::exception();
      return *cbegin();
  }
  const T &back() const {
      if (empty()) throw std::exception();
      const_iterator it = cend();
      it--;
      return *it;
  }

  iterator begin() { return iterator(node->_nxt); }
  const_iterator cbegin() const {  return const_iterator(node->_nxt); }
  iterator end() { return iterator(node); }
  const_iterator cend() const { return const_iterator(node); }
  virtual bool empty() const {
      return node->_nxt == node;
  } //wow,so ingenious the trick is

  virtual size_t size() const {
      int cnt = 0;
      for (const_iterator it = cbegin(); it != cend(); it++)
          cnt++;
      return cnt;
  }//可考虑优化时间

  virtual void clear() {
      listnode* ptr = node->_nxt;
      while (ptr != node){
          listnode* tmp = ptr;
          ptr = ptr->_nxt;
          destroy_node(tmp);
      }
      node->_nxt = node;
      node->_pre = node;
  }

  virtual iterator insert(iterator pos, const T &value) {
      listnode* ptr = create_node(value);
      listnode* self = pos.node;
      ptr->_nxt = self;
      ptr->_pre = self->_pre;
      self->_pre->_nxt = ptr;
      self->_pre = ptr;
      return iterator(ptr);
  }

  virtual iterator erase(iterator it) {
      if (empty()) throw std::exception();
      listnode* pos = it.node;
      listnode* nxt = pos->_nxt, *pre = pos->_pre;
      pre->_nxt = nxt;
      nxt->_pre = pre;
      destroy_node(pos);
      return iterator(nxt);
  }

  void push_back(const T &value) {
      insert(end(), value);
  }
  void pop_back() {
      iterator tmp = end();
      tmp--;
      erase(tmp);
  }
  void push_front(const T &value) {
      insert(begin(), value);
  }
  void pop_front() {
      erase(begin());
  }
};

} // namespace sjtu

#endif // SJTU_LIST_HPP
