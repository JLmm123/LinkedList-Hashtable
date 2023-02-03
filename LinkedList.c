/*
 * Copyright ©2023 Travis McGaha.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Pennsylvania
 * CIT 5950 for use solely during Spring Semester 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>
#include <stdlib.h>

#include "LinkedList.h"
#include "LinkedList_priv.h"


///////////////////////////////////////////////////////////////////////////////
// LinkedList implementation.

LinkedList* LinkedList_Allocate(void) {

  // TODO: allocate the LinkedList struct and initialize the newly allocated record structure.
  LinkedList* ll= malloc(sizeof(LinkedList));

  if(ll == NULL) return NULL;  // you may want to change this

  ll->num_elements = 0;
  ll->head = NULL;
  ll->tail = NULL;

  return ll;
}

void LinkedList_Free(LinkedList *list,
                     LLPayloadFreeFnPtr payload_free_function) {

  // TODO: sweep through the list and free all of the nodes' payloads
  // (using the payload_free_function supplied as an argument) and
  // the nodes themselves. 

  // free the LinkedList
  while(list -> head != NULL){
    payload_free_function(list -> head -> payload);
    LinkedListNode* temp = list->head;
    list->head = list->head->next;
    free(temp);

  }
  free(list);
}

int LinkedList_NumElements(LinkedList *list) {
  return list->num_elements;
}

void LinkedList_Push(LinkedList *list, LLPayload_t payload) {
  // TODO: implement LinkedList_Push
  LinkedListNode* ln = malloc(sizeof(LinkedListNode));

  ln->payload = payload;

  if (list->num_elements == 0) {
    ln->next = ln->prev = NULL;
    list->head = list->tail = ln;
    list->num_elements = 1U;
  }else{
      ln->next = list->head;
      ln->prev = NULL;
      list->head->prev = ln;
      list->head = ln;
      list->num_elements += 1;
  }

}

bool LinkedList_Pop(LinkedList *list, LLPayload_t *payload_ptr) {
  // TODO: implement LinkedList_Pop.  Make sure you test for
  // and empty list and fail.  If the list is non-empty, there
  // are two cases to consider: (a) a list with a single element in it
  // and (b) the general case of a list with >=2 elements in it.
  // Be sure to call free() to deallocate the memory that was
  // previously allocated by LinkedList_Push().
   
  // if nothing 
  if (list->num_elements == 0)
    return false;

  // make a copy of pl
  *payload_ptr = list->head->payload;
  //the head of the list
  LinkedListNode* temp = list->head;

  //only has one elements
  if (list->num_elements == 1) {
    list->head = NULL;
    list->tail = NULL;
  } else {
    // else
    list->head = list->head->next;
    list->head->prev = NULL;
  }

  list->num_elements -= 1;
  free(temp);

  //success
  return true;  // you may need to change this return value
}

void LinkedList_Append(LinkedList *list, LLPayload_t payload) {
  // TODO: implement LinkedList_Append.  It's kind of like
  // LinkedList_Push, but obviously you need to add to the end
  // instead of the beginning.
  LinkedListNode* ln = malloc(sizeof(LinkedListNode));
  if (ln == NULL) {
    // failure of malloc
    return ;
  }

  // set the payload
  ln->payload = payload;

  // list is empty
  if (list->num_elements == 0) {
    ln->next = ln->prev = NULL;
    list->head = list->tail = ln;
    list->num_elements = 1U;

  }else{
      //if the list is not empty
    ln->next = NULL;
    ln->prev = list->tail;
    list->tail->next = ln;
    list->tail = ln;
    list->num_elements += 1;

  }

}

bool LinkedList_Slice(LinkedList *list, LLPayload_t *payload_ptr) {
  // TODO: implement LinkedList_Slice.
  // list is empty
  if (list->num_elements == 0)
    return false;

  // make a copy of payload
  *payload_ptr = list->tail->payload;
  LinkedListNode* temp = list->tail;

  if (list->num_elements == 1) {
    //only has one node
    list->head = NULL;
    list->tail = NULL;
  } else {
    // else
    list->tail = list->tail->prev;
    list->tail->next = NULL;
  }

  list->num_elements -= 1;
 
  free(temp);

  
  return true;
    // you may need to change this return value
}


// this function is completed for you
void LinkedList_Sort(LinkedList *list, bool ascending,
                     LLPayloadComparatorFnPtr comparator_function) {
  if (list->num_elements < 2) {
    // No sorting needed.
    return;
  }

  // We'll implement bubblesort! Nnice and easy, and nice and slow :)
  int swapped;
  do {
    LinkedListNode *curnode;

    swapped = 0;
    curnode = list->head;
    while (curnode->next != NULL) {
      int compare_result = comparator_function(curnode->payload,
                                               curnode->next->payload);
      if (ascending) {
        compare_result *= -1;
      }
      if (compare_result < 0) {
        // Bubble-swap the payloads.
        LLPayload_t tmp;
        tmp = curnode->payload;
        curnode->payload = curnode->next->payload;
        curnode->next->payload = tmp;
        swapped = 1;
      }
      curnode = curnode->next;
    }
  } while (swapped);
}


///////////////////////////////////////////////////////////////////////////////
// LLIterator implementation.

LLIterator* LLIterator_Allocate(LinkedList *list) {
  // TODO: implement

  LLIterator* li = malloc(sizeof(LLIterator));
  if (li == NULL) {
    // malloc failure
    return NULL;
  }

  // set up the iterator.
  li->list = list;
  li->node = list->head;
  
  return li;
 
}

// implemented for you
void LLIterator_Free(LLIterator *iter) {
  free(iter);
}

bool LLIterator_IsValid(LLIterator *iter) {
  // TODO: implement

  if (iter->node == NULL) return false;  // no

  return true;  // yes

}

bool LLIterator_Next(LLIterator *iter) {
  // TODO: try to advance iterator to the next node and return true if
  // you succeed and are now on a new node, false otherwise
    
  iter->node = iter->node->next;

  if (LLIterator_IsValid(iter)) {
    return true;
  }

  return false;
 // you may need to change this return value
}

void LLIterator_Get(LLIterator *iter, LLPayload_t *payload) {
  // TODO: implement
  if (LLIterator_IsValid(iter)) {
      *payload = iter->node->payload;
  }
}

bool LLIterator_Remove(LLIterator *iter,
                       LLPayloadFreeFnPtr payload_free_function) {

  // TODO: implement LLIterator_Remove.  This is the most
  // complex function you'll build.  There are several cases
  // to consider:
  // - degenerate case: the list becomes empty after deleting.
  // - degenerate case: iter points at head
  // - degenerate case: iter points at tail
  // - fully general case: iter points in the middle of a list,
  //                       and you have to "splice".
  //
  // Be sure to call the payload_free_function to free the payload
  // the iterator is pointing to, and also free any LinkedList
  // data structure element as appropriate.

  // free the current  payload
  payload_free_function(iter->node->payload);
  // pointcurrent node
  LinkedListNode* temp = iter->node;

  if (iter->list->num_elements == 1) {
    iter->node = NULL;
    iter->list->num_elements -= 1;
    iter->list->head = NULL;
    iter->list->tail = NULL;

    // free the node
    free(temp);
    //the list is empty
    return false;
  }

  iter->list->num_elements -= 1;

  if (iter->node->prev == NULL) {
    // case 1: iter point the head
    iter->node = iter->node->next;
    iter->list->head = iter->node;
    iter->node->prev = NULL;
  } else if (iter->node->next == NULL) {
    // case 2: iter point the tail
    iter->node = iter->node->prev;
    iter->list->tail = iter->node;
    iter->node->next = NULL;
  } else {
    // case 3: iter points at the middle of the list
    iter->node->prev->next = iter->node->next;
    iter->node->next->prev = iter->node->prev;
    iter->node = iter->node->next;
  }

  // free the node
  free(temp);

  return true;  // you may need to change this return value
}


// Implemented for you
void LLIterator_Rewind(LLIterator *iter) {
  iter->node = iter->list->head;
}
