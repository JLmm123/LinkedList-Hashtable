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

#ifndef HW0_LINKEDLIST_PRIV_H_
#define HW0_LINKEDLIST_PRIV_H_

#include "./LinkedList.h"  // for LinkedList and LLIterator

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Internal structures and helper functions for our LinkedList implementation.
//
// These would typically be located in LinkedList.c; however, we have broken
// them out into a "private .h" so that our unittests can access them.  This
// allows our test code to peek inside the implementation to verify correctness.
//
// Customers should not include this file or assume anything based on
// its contents.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


// A single node within a linked list.
//
// A node contains next and prev pointers as well as a customer-supplied
// payload pointer.
typedef struct ll_node {
  LLPayload_t     payload;  // customer-supplied payload pointer
  struct ll_node *next;     // next node in list, or NULL
  struct ll_node *prev;     // prev node in list, or NULL
} LinkedListNode;

// The entire linked list.
//
// We provided a struct declaration (but not definition) in LinkedList.h;
// this is the associated definition.  This struct contains metadata
// about the linked list.
typedef struct ll {
  int               num_elements;  //  # elements in the list
  LinkedListNode   *head;  // head of linked list, or NULL if empty
  LinkedListNode   *tail;  // tail of linked list, or NULL if empty
} LinkedList;

// A linked list iterator.
//
// We expose the struct declaration in LinkedList.h, but not the definition,
// similar to what we did above for the linked list itself.
typedef struct ll_iter {
  LinkedList       *list;  // the list we're for
  LinkedListNode   *node;  // the node we are at, or NULL if broken
} LLIterator;


#endif  // HW0_LINKEDLIST_PRIV_H_
