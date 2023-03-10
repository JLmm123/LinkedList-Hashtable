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
#include <stdint.h>

#include "HashTable.h"
#include "HashTable_priv.h"

///////////////////////////////////////////////////////////////////////////////
// Internal helper functions.
//
#define INVALID_IDX -1

// Grows the hashtable (ie, increase the number of buckets) if its load
// factor has become too high.
static void MaybeResize(HashTable *ht);

static bool BucketHasKey(LLIterator *iter, 
                  HTKey_t key,
                  HTKeyValue_t **keyvalue);

// Implemented for you
int HashKeyToBucketNum(HashTable *ht, HTKey_t key) {
  return key % ht->num_buckets;
}

// Deallocation functions that do nothing.  Useful if we want to deallocate
// the structure (eg, the linked list) without deallocating its elements or
// if we know that the structure is empty.
static void LLNoOpFree(LLPayload_t freeme) { }
static void HTNoOpFree(HTValue_t freeme) { }


///////////////////////////////////////////////////////////////////////////////
// HashTable implementation.

// Implemented for you
HTKey_t FNVHash64(unsigned char *buffer, int len) {
  // This code is adapted from code by Landon Curt Noll
  // and Bonelli Nicola:
  //     http://code.google.com/p/nicola-bonelli-repo/
  static const uint64_t FNV1_64_INIT = 0xcbf29ce484222325ULL;
  static const uint64_t FNV_64_PRIME = 0x100000001b3ULL;
  unsigned char *bp = (unsigned char *) buffer;
  unsigned char *be = bp + len;
  uint64_t hval = FNV1_64_INIT;

  // FNV-1a hash each octet of the buffer.
  while (bp < be) {
    // XOR the bottom with the current octet.
    hval ^= (uint64_t) * bp++;
    // Multiply by the 64 bit FNV magic prime mod 2^64.
    hval *= FNV_64_PRIME;
  }
  return hval;
}

// Implemented for you
HashTable* HashTable_Allocate(int num_buckets) {
  HashTable *ht;
  int i;

  // Allocate the hash table record.
  ht = (HashTable *) malloc(sizeof(HashTable));
  if (!ht) {
    return ht;
  }

  // Initialize the record.
  ht->num_buckets = num_buckets;
  ht->num_elements = 0;
  ht->buckets = (LinkedList **) malloc(num_buckets * sizeof(LinkedList *));
  for (i = 0; i < num_buckets; i++) {
    ht->buckets[i] = LinkedList_Allocate();
  }

  return ht;
}

// Implemented for you
void HashTable_Free(HashTable *table,
                    ValueFreeFnPtr value_free_function) {
  int i;

  // Free each bucket's chain.
  for (i = 0; i < table->num_buckets; i++) {
    LinkedList *bucket = table->buckets[i];
    HTKeyValue_t *kv;

    // Pop elements off the chain list one at a time.  We can't do a single
    // call to LinkedList_Free since we need to use the passed-in
    // value_free_function -- which takes a HTValue_t, not an LLPayload_t -- to
    // free the caller's memory.
    while (LinkedList_NumElements(bucket) > 0) {
      LinkedList_Pop(bucket, (LLPayload_t *)&kv);
      value_free_function(kv->value);
      free(kv);
    }
    // The chain is empty, so we can pass in the
    // null free function to LinkedList_Free.
    LinkedList_Free(bucket, LLNoOpFree);
  }

  // Free the bucket array within the table, then free the table record itself.
  free(table->buckets);
  free(table);
}

// Implemented for you
int HashTable_NumElements(HashTable *table) {
  return table->num_elements;
}



//helper method, see if the bucket has the key
static bool BucketHasKey(LLIterator *iter, 
                  HTKey_t key,
                  HTKeyValue_t **keyvalue){
  while(true){
    if(!LLIterator_IsValid(iter)) break;

    LLIterator_Get(iter, (void **) keyvalue);

    //if found
    if((*keyvalue) -> key == key) return true;
    
    //move to the next one
    LLIterator_Next(iter);
  }

  return false;

}

bool HashTable_Insert(HashTable *table,
                      HTKeyValue_t newkeyvalue,
                      HTKeyValue_t *oldkeyvalue) {
  int bucket;
  
  MaybeResize(table);

  // Calculate which bucket and chain we're inserting into.
  bucket = HashKeyToBucketNum(table, newkeyvalue.key);
  LinkedList *chain = table->buckets[bucket];

  // STEP 1: finish the implementation of InsertHashTable.
  // This is a fairly complex task, so you might decide you want
  // to define/implement a helper function that helps you find
  // and optionally remove a key within a chain, rather than putting
  // all that logic inside here.  You might also find that your helper
  // can be reused in steps 2 and 3.
  HTKeyValue_t *newpl = (HTKeyValue_t *) malloc(sizeof(HTKeyValue_t));

  //if(newpl == NULL) return false;

  newpl -> key = newkeyvalue.key;
  newpl -> value = newkeyvalue.value;

  if (LinkedList_NumElements(chain) == 0) {
    // push newkeyvalue to the bucket, return 0 if push failed
    LinkedList_Push(chain, newpl);

    table->num_elements += 1;
    // push succeeded
    return false;
  }

  LLIterator *lliter = LLIterator_Allocate(chain);

  HTKeyValue_t *oldpl;

  //check if bucket 
if (BucketHasKey(lliter, newkeyvalue.key, &oldpl)) {
    //add to the linkedlist
    LinkedList_Push(chain, newpl);

    // copy the old key
    oldkeyvalue->key = oldpl->key;
    oldkeyvalue->value = oldpl->value;

    // free the ollpayload since we malloc when we inserted it
    free(oldpl);

    // detele oldkeyvalue from the bucket
    LLIterator_Remove(lliter, &LLNoOpFree);

    LLIterator_Free(lliter);
   

    // append succeed
    return true;
  }
  
  LLIterator_Free(lliter);
  // push newkeyvalue to the bucket
  LinkedList_Push(chain, newpl);

  // push succeed
  table->num_elements += 1;

  return false;  // you may need to change this return value
}





bool HashTable_Find(HashTable *table,
                    HTKey_t key,
                    HTKeyValue_t *keyvalue) {
  // STEP 2: implement HashTable_Find.

  HTKey_t bucket;
  HTKeyValue_t *payload;

  bucket = HashKeyToBucketNum(table, key);
  LinkedList *chain = table -> buckets[bucket];

  //if this list contains zero elements
  if(LinkedList_NumElements(chain) == 0) return false;

  //if this list contains elements
  LLIterator *lliter = LLIterator_Allocate(chain);

  //if this bucket has this key, then store the payload into return parameter, keyvalue
  if(BucketHasKey(lliter, key, &payload)){
    keyvalue -> key = payload ->key;
    keyvalue -> value = payload -> value;

    LLIterator_Free(lliter);
    return true;
  }

  LLIterator_Free(lliter);
  return false;  // you may need to change this return value
}

bool HashTable_Remove(HashTable *table,
                      HTKey_t key,
                      HTKeyValue_t *keyvalue) {
  // STEP 3: implement HashTable_Remove.

  HTKey_t bucket;
  HTKeyValue_t *payload;

  bucket = HashKeyToBucketNum(table, key);
  LinkedList *chain = table -> buckets[bucket];

  //if this list contains zero elements
  if(LinkedList_NumElements(chain) == 0) return false;

  //if this list contains elements
  LLIterator *lliter = LLIterator_Allocate(chain);

  //if this bucket has this key, then store the payload into return parameter, keyvalue
  if(BucketHasKey(lliter, key, &payload)){
    keyvalue -> key = payload -> key;
    keyvalue -> value = payload -> value;

    free(payload);

    LLIterator_Remove(lliter, &LLNoOpFree);
    LLIterator_Free(lliter);

    table->num_elements -= 1;

    return true;
  }

  LLIterator_Free(lliter);
  return false;  // you may need to change this return value
}


///////////////////////////////////////////////////////////////////////////////
// HTIterator implementation.

// Implemented for you
HTIterator* HTIterator_Allocate(HashTable *table) {
  HTIterator *iter;
  int         i;

  iter = (HTIterator *) malloc(sizeof(HTIterator));

  // If the hash table is empty, the iterator is immediately invalid,
  // since it can't point to anything.
  if (table->num_elements == 0) {
    iter->ht = table;
    iter->bucket_it = NULL;
    iter->bucket_idx = INVALID_IDX;
    return iter;
  }

  // Initialize the iterator.  There is at least one element in the
  // table, so find the first element and point the iterator at it.
  iter->ht = table;
  for (i = 0; i < table->num_buckets; i++) {
    if (LinkedList_NumElements(table->buckets[i]) > 0) {
      iter->bucket_idx = i;
      break;
    }
  }
  iter->bucket_it = LLIterator_Allocate(table->buckets[iter->bucket_idx]);
  return iter;
}

// Implemented for you
void HTIterator_Free(HTIterator *iter) {
  if (iter->bucket_it != NULL) {
    LLIterator_Free(iter->bucket_it);
    iter->bucket_it = NULL;
  }
  free(iter);
}

bool HTIterator_IsValid(HTIterator *iter) {
  // STEP 4: implement HTIterator_IsValid.
  if( iter -> ht -> num_elements == 0 ) return false;
  if (iter -> bucket_idx >= iter->ht->num_buckets) return false;
  if( (!LLIterator_IsValid(iter -> bucket_it) ))
  return false;

  return true;  // you may need to change this return value
}

bool HTIterator_Next(HTIterator *iter) {
  // STEP 5: implement HTIterator_Next.
  if(HTIterator_IsValid(iter) == false) return false;
  if(iter == NULL) return false;
  if(iter -> ht == NULL) return false;

  //if the lliterator does not past the end, move to the next one
  if(LLIterator_Next(iter -> bucket_it)) {
    return true;}
  else{
     int i = 0;
    //locate to the next non-empty bucket
    for (i = iter->bucket_idx + 1; i < iter->ht->num_buckets; i++) {
    
    if (LinkedList_NumElements(iter->ht->buckets[i]) > 0) {
      iter->bucket_idx = i;
      break;
    }

  }

  //if there is no more non empty buckets ahead
  if (i == (iter->ht->num_buckets)) {
    return false;
  }
  }
  

  LLIterator_Free(iter->bucket_it);
  iter->bucket_it = LLIterator_Allocate(iter->ht->buckets[iter->bucket_idx]);


  return true;  // you may need to change this return value
}

bool HTIterator_Get(HTIterator *iter, HTKeyValue_t *keyvalue) {
  // STEP 6: implement HTIterator_Get.
  HTKeyValue_t *payload;
  //if(iter -> ht == NULL) return false;

  if(HTIterator_IsValid(iter)){
      LLIterator_Get(iter -> bucket_it, (void **) &payload);

      keyvalue->key = payload->key;
      keyvalue->value = payload->value;

      return true;
  }
  return false;  // you may need to change this return value
}

// Implemented for you
bool HTIterator_Remove(HTIterator *iter, HTKeyValue_t *keyvalue) {
  HTKeyValue_t kv;

  // Try to get what the iterator is pointing to.
  if (!HTIterator_Get(iter, &kv)) {
    return false;
  }

  // Advance the iterator.  Thanks to the above call to
  // HTIterator_Get, we know that this iterator is valid (though it
  // may not be valid after this call to HTIterator_Next).
  HTIterator_Next(iter);

  // Lastly, remove the element.  Again, we know this call will succeed
  // due to the successful HTIterator_Get above.
  HashTable_Remove(iter->ht, kv.key, keyvalue);

  return true;
}

// Implemented for you
static void MaybeResize(HashTable *ht) {
  HashTable *newht;
  HashTable tmp;
  HTIterator *it;

  // Resize if the load factor is > 3.
  if (ht->num_elements < 3 * ht->num_buckets)
    return;

  // This is the resize case.  Allocate a new hashtable,
  // iterate over the old hashtable, do the surgery on
  // the old hashtable record and free up the new hashtable
  // record.
  newht = HashTable_Allocate(ht->num_buckets * 9);

  // Loop through the old ht copying its elements over into the new one.
  for (it = HTIterator_Allocate(ht);
       HTIterator_IsValid(it);
       HTIterator_Next(it)) {
    HTKeyValue_t item, unused;

    HTIterator_Get(it, &item);
    HashTable_Insert(newht, item, &unused);
  }

  // Swap the new table onto the old, then free the old table (tricky!).  We
  // use the "no-op free" because we don't actually want to free the elements;
  // they're owned by the new table.
  tmp = *ht;
  *ht = *newht;
  *newht = tmp;

  // Done!  Clean up our iterator and temporary table.
  HTIterator_Free(it);
  HashTable_Free(newht, &HTNoOpFree);
}
