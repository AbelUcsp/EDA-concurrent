// Copyright
#ifndef SOURCE_SKIP_LIST_SKIP_LIST_HPP_
#define SOURCE_SKIP_LIST_SKIP_LIST_HPP_

#include <functional>
#include <iostream>
#include <random>
#include <vector>

#include <mutex>
#include <thread>
#include <chrono>

namespace ADE {
namespace Concurrent {

template <typename Type, class Comparator = std::less<Type>>
class ConcurrentSkipList {
 public:
  typedef Type data_type;
  Comparator compare_;
  std::mutex locker;
  bool get_random_bool() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, 1);
    return distribution(gen);
  }

  struct Node {
    bool marked;
    bool fullyLinked;
    //std::mutex locker;

    Node(unsigned int level, const data_type& data)
        : forward_(level, nullptr), value_(data) {
        marked = false;
        fullyLinked = false;
        }
    std::vector<Node*> forward_;
    data_type value_;
  };

  explicit ConcurrentSkipList(unsigned int max_level = 16)
      : max_level_(max_level),
        current_level_(1),
        header_(max_level_, data_type()) {}

  bool insert(const data_type& data) {
    std::vector<Node*> predecessor;
    Node* succ;// = it;
    while(true){
      
      Node* it = find(data, &predecessor);
      if(it != nullptr && it->value_ == data){
        if(it->forward_[0]){
          succ = it->forward_[0];
        }
      }
      else{
        if(predecessor[0]->forward_[0] != nullptr && predecessor[0]->forward_[0]->value_ > data){
          succ = predecessor[0]->forward_[0];
        }
      }//end succesor

      if (it != nullptr && it->value_ == data)
        return false;


      if(succ){
        if(!succ->marked){  
          if(!succ->fullyLinked){ 
            if(!succ->fullyLinked )
              return false; 
          }
        }
      }

      if (it != nullptr){  
        if(!predecessor[0]->marked){    //no siendo eliminado
          if(!predecessor[0]->fullyLinked ) //no terminado
            return false; 
        }
        //continue;
      }
      
      this->locker.lock();
      unsigned int level = 1;
      while (get_random_bool() && level < max_level_) {
        ++level;
      }

      if (current_level_ < level) {
        predecessor[current_level_] = &header_;
        level = ++current_level_;
      }

      Node* new_node = new Node(level, data_type(data));
      for (unsigned int i = 0; i < level; ++i) {
        new_node->forward_[i] = predecessor[i]->forward_[i];
        predecessor[i]->forward_[i] = new_node;
      }
      new_node->fullyLinked = true;
    

      this->locker.unlock();
      return true;
    }

  }//end insert

  bool remove(const data_type& data) {
    std::vector<Node*> predecessor;
    Node* it = find(data, &predecessor); //para poder modificar
    Node* succ;// = it;

    if(it != nullptr && it->value_ == data){
      succ = it->forward_[0];
    }else{
      succ = it;
    }

    int tmp;
    if(it != nullptr && it->value_ == data){
      for(int i=0; i<=current_level_; i++) 
      { 
        if(predecessor[i]->forward_[i] != it) 
          break; 
    
        predecessor[i]->forward_[i] = it->forward_[i]; 
      } 
      tmp = it->value_;
      delete it;
    }
    //int tmp = it->value_;

    return tmp = data;
  }//end remove


  void debug_print() 
  { 
      std::cout<<"\nSkip_List print"<<"\n"; 
      for(int i=0;i<current_level_;i++) 
      { 
          Node *node = header_.forward_[i]; 
          std::cout<<i<<"-> "; 
          int j = 0;
          while(node != nullptr) 
          { 
              j++;
              if(j==node->value_){
              std::cout<<node->value_<<" "; 
              node = node->forward_[i]; 
              }
              
          } 
          std::cout<<"\n"; 
      } 
  }
  /*
  unsigned int size() const {
    unsigned int size = 0;
    Node* it = header_.forward_[0];
    while (it != nullptr) {
      ++size;
      it = it->forward_[0];
    }
    return size;
  }
  *

  void debug_print() {
    Node* it = &header_;
    while (it->forward_[0] != nullptr) {
      for (unsigned int i = 0; i < it->forward_.size(); ++i) {
        std::cout << it->forward_[0]->value_ << " ";
      }
      std::cout << std::endl;
      it = it->forward_[0];
    }
  }
*/
  virtual ~ConcurrentSkipList() {
    Node* it = header_.forward_[0];
    while (it) {
      Node* ptr = it->forward_[0];
      delete it;
      it = ptr;
    }
  }

 private:
  Node* find(const data_type& data, std::vector<Node*>* predecessor) {
    predecessor->resize(max_level_, nullptr);
    Node* it = &header_;
    int i = current_level_ - 1;
    for ( ; 0 <= i; --i) {
      while (it->forward_[i] != nullptr &&
             compare_(it->forward_[i]->value_, data)) {
        it = it->forward_[i];
      }
      (*predecessor)[i] = it;
      
    }
    it = it->forward_[0];
    
      //std::cout << "succ: "<< (*succ)[0]->value_ <<std::endl;
    return it;
  }

  unsigned int max_level_;
  unsigned int current_level_;
  Node header_;
};
}  // namespace Sequential
}  // namespace ADE

#endif  // SOURCE_SKIP_LIST_SKIP_LIST_HPP_
