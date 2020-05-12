// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $

#include "listmap.h"
#include "debug.h"


//
//
//
/////////////////////////////////////////////////////////////////
// Operations on listmap
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));

   while(!(empty())){
      erase(begin());
   }

}
  

// iterator listmap::insert (const value_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);

      //Empty List. Insert only 1 node
      if(empty() == true){
          anchor()->next = new node(anchor(), anchor(), pair);
          anchor()->prev = anchor()->next;
      }

      //More than 1 node.
      else{

         //Check to see if key already exists
         auto iter = find(pair.first);

         //A match is found just replace value
         if(iter != anchor()){
            iter->second = pair.second;
            return iter;
         }

         //No matches for key
         //insert it lexographically
         else{

            auto currNode = anchor()->next;

            //Finding node to work with
            while(currNode != anchor() &&
                      (less(currNode->value.first, pair.first))){
               currNode = currNode->next;
            }

            //Made it to the end. append the newNode.
            if(currNode == anchor()){
               node *newNode = new node(anchor(), anchor()->prev, pair);
               anchor()->prev->next = newNode;
               anchor()->prev = newNode;
               return newNode;
            }

            //Only one node. prepend the newNode.
            else if(currNode == anchor()->next){
               node *newNode = new node(anchor()->next, anchor(), pair);
               anchor()->next->prev = newNode;
               anchor()->next = newNode;
               return newNode;
            }

            //insert somewhere in the middle
            else {
               node *newNode = new node(currNode, currNode->prev, pair);
               currNode->prev->next = newNode;
               currNode->prev = newNode;
               return newNode;
            }

         }

      }

   return iterator();
}

//
// listmap::find(const key_type&)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);

   auto currNode = anchor()->next;

   while(currNode != anchor()){

      //Found a match for the key
      if(!(less(currNode->value.first, that)) &&
            !(less(that, currNode->value.first))){

         return currNode;
      }

      currNode = currNode->next;
   }


   //If not found return the anchor 
   return anchor();
}

//
// iterator listmap::erase (iterator position)
//
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);

   // If the position youre at is the same as the next
   // there is only 1 node in the list
   if(position.where == position.where->next){
      delete position.where;
      return anchor();
   }

   // There are multiple nodes so you have to make sure
   // to link up the rest of the nodes
   else{

      //Link up the prev node with the next node

      (position.where)->next->prev = (position.where)->prev;
      (position.where)->prev->next = (position.where)->next;

      //Save the next node to return back
      node* nextNode = (position.where)->next;

      //Finally delete node and return nextNode
      delete position.where;
      return nextNode;
   }

   return iterator();
}
