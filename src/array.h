#pragma once

#include <cstdlib>
#include "object.h"
#include "string.h"

/**
 * Assuming an array is a java-style arraylist. Stores objects by index.
 */
class Array : public Object
{
public:
  // elements_ is a pointer to a Object* that we stored
  Object** elements_;
  // the number of items stored in the array
  size_t size_;
  
  // constructor
  // initializes size_ to 0
  // initializes elements_ to nullptr
  Array()
  {
    size_ = 0;
    elements_ = nullptr;
  }

  // destructor
  ~Array() {}

  // putting an item at the end of the array
  virtual void append(Object* oo)
  {
    // allocate memory for result array with larger size
    Object** res = new Object*[size_ + 1];
    // if not empty array, put item to end of array
    if (size_ != 0)
    {
      for (size_t i = 0; i < size_; i++)
      {
        res[i] = elements_[i];
      }
      res[size_] = oo;
      // if empty array, set first element in array
    }
    else
    {
      res[0] = oo;
    }

    // set elements to the new result array
    elements_ = res;

    // increase size of array
    size_ += 1;
  }

  // get the n'th item in the array
  Object* get(size_t nn)
  {
    // if index not within bounds, return nullptr
    if (size_ == 0 || nn >= size_)
    {
      return nullptr;
    }
    // else return element of given index
    else
    {
      for (size_t i = 0; i < size_; i++)
      {
        if (i == nn)
        {
          return elements_[i];
        }
      }
    }
  }

  // remove the n'th item in the array
  // returning the removed item to the caller
  Object* remove(size_t nn)
  {
    // if size not within bounds, return nullptr
    if (size_ == 0 || nn >= size_)
    {
      return nullptr;
    }
    // else remove the item at given index
    else
    {
      // make variable for removed to be returned
      Object* removed;
      // allocate memory for result array with one less size since we remove
      Object** res = new Object*[size_ - 1];
      // copy the elements up to nn in result array
      for (size_t i = 0; i < nn; i++)
      {
        res[i] = elements_[i];
      }
      // set remove to the last element
      removed = elements_[nn];
      // copy elements after nn to result array
      for (size_t j = nn + 1; j < size_; j++)
      {
        res[j - 1] = elements_[j];
      }
      // set elements to result array
      elements_ = res;
      // decrease size of array
      size_ -= 1;
      return removed;
    }
  }

  // returns the length of the array.
  virtual size_t length()
  {
    return size_;
  }

  // set n'th element to the given object
  // returns the replace Object.
  virtual Object* set(size_t nn, Object* oo)
  {
    // if size not within bounds, return nullptr
    if (size_ == 0 || nn >= size_)
    {
      return nullptr;
    }
    // else set nth element
    else
    {
      Object* res = elements_[nn];
      elements_[nn] = oo;
      return res;
    }
  }

  // get the index of an object in the array
  virtual size_t index_of(Object* oo)
  {
    // if empty array, return arbitrary large number
    if (size_ == 0)
    {
      return 500;
    }
    // return a value greater than size if index not found
    else
    {
      for (size_t i = 0; i < size_; i++)
      {
        if (oo == elements_[i])
        {
          return i;
        }
      }
      return size_ + 500;
    }
  }

  // reset and delete every element in the array
  virtual void clear()
  {
    // if anything in array, remove it
    if (size_ > 0)
    {
      for (size_t i = 0; i < size_; i++)
      {
        remove(i);
      }
    }
    // set size to 0
    size_ = 0;
  };

  // Compares other with this array for equality.
  bool equals(Object* oo)
  {
    // set false if nullptr
    if (oo == nullptr)
    {
      return false;
    }
    // cast oo to Array
    Array* a = dynamic_cast<Array*>(oo);
    if (a == nullptr)
    {
      return false;
    }
    // return false if length is not equal to this array
    if (a->length() != size_)
    {
      return false;
    }

    else
    {
      // if size is 0, both arrays equal
      if (size_ == 0)
      {
        return true;
      }
      // iterate and check if all elements in both arrays are equal
      for (size_t i = 0; i < size_; i++)
      {
        if (!elements_[i]->equals(a->elements_[i]))
        {
          return false;
        }
      }
      return true;
    }
  }

  // compute by adding hash from each element
  size_t hash()
  {
    size_t res = 0;
    for (size_t i = 0; i < size_; i++)
    {
      res += elements_[i]->hash_me();
    }
    return res;
  }
};

/**
 * Array that supports primitive type int.
 */
class IntArray : public Array
{
public:
  // elements_ is a pointer to a int* that we stored
  int* elements_;
  // size of the array
  size_t size_;

  // constructor
  IntArray()
  {
    // initialize array size to 0
    size_ = 0;
    // initialize elements to nullptr
    elements_ = nullptr;
  }

  // destructor
  ~IntArray() {}

  // putting an item at the end of the array
  void append(int oo)
  {
    // allocate memory for result array with larger size
    int* res = new int[size_ + 1];
    // if empty array, set first element of result array
    if (size_ == 0)
    {
      res[0] = oo;
    }
    // else copy elements to result and set last item to given
    else
    {
      for (size_t i = 0; i < size_; i++)
      {
        res[i] = elements_[i];
      }
      res[size_] = oo;
    }
    // set elements field to result
    elements_ = res;
    // increase size
    size_ += 1;
  }

  // get the n'th item in the array
  int get(size_t nn)
  {
    // if size of array out of bounds, return NULL
    if (size_ == 0 || nn > size_)
    {
      return NULL;
    }
    // else return element of given index
    else
    {
      return elements_[nn];
    }
  }

  // remove the n'th item in the array
  // returning the removed item to the caller
  int remove(size_t nn)
  {
    // if out of bounds, return NULL
    if (size_ == 0 || nn > size_)
    {
      return NULL;
    }
    else
    {
      // allocate memory for temp array with one less than size
      int* temp = new int[size_ - 1];
      // set the result to be returned to the element at given index
      int res = elements_[nn];
      // set temp to elements up to the index of removed
      for (size_t i = 0; i < nn; i++)
      {
        temp[i] = elements_[i];
      }
      // set temp to elements after index of removed
      for (size_t j = nn + 1; j < size_; j++)
      {
        temp[j - 1] = elements_[j];
      }
      // decrease array size
      size_ -= 1;
      return res;
    }
  }

  // set n'th element to the given element
  // returns the replace Object.
  int set(size_t nn, int oo)
  {
    // if out of bounds, return NULL
    if (size_ == 0 || nn > size_)
    {
      printf("size =0\n");
      return NULL;
    }
    else
    {
      printf("size %i\n", size_);
      int res = elements_[nn];
      elements_[nn] = oo;
      return res;
    }
  }

  // return size of array
  size_t length()
  {
    return size_;
  }

  // get the index of an element in the array
  size_t index_of(int oo)
  {
    // if empty array, return arbitrary large number
    if (size_ == 0)
    {
      return 500;
    }
    // return a value greater than size if index not found
    else
    {
      for (size_t i = 0; i < size_; i++)
      {
        if (oo == elements_[i])
        {
          return i;
        }
      }
      return size_ + 500;
    }
  }

  // reset and delete every element in the array
  void clear()
  {
    // if anything in array, remove it
    if (size_ > 0)
    {
      for (size_t i = 0; i < size_; i++)
      {
        remove(i);
      }
    }
    size_ = 0;
  };

  // Compares other with this array for equality.
  bool equals(Object* oo)
  {
    // set false if nullptr
    if (oo == nullptr)
    {
      return false;
    }
    // cast oo to IntArray
    IntArray* ia = dynamic_cast<IntArray*>(oo);
    if (ia == nullptr)
    {
      return false;
    }
    // if length is not equal, return false
    if (ia->length() != size_)
    {
      return false;
    }
    else
    {
      // if size is 0, both arrays empty so return true
      if (size_ == 0)
      {
        return true;
      }
      // if any element not equal, return false
      for (size_t i = 0; i < size_; i++)
      {
        if (elements_[i] != ia->elements_[i])
        {
          return false;
        }
      }
      // else return true
      return true;
    }
  }

  // compute by adding hash from each element
  size_t hash()
  {
    size_t hash = 0;
    for (size_t i = 0; i < size_; i++)
    {
      hash += elements_[i];
    }
    return hash;
  }
};

/**
 * Array that supports primitive type float.
 */
class FloatArray : public Array
{
public:
  // size of array
  size_t size_;
  // elements_ is a pointer to a int* that we stored
  float* elements_;

  // constructor
  FloatArray()
  {
    // initialize size to 0
    size_ = 0;
    // initialize elements_ to nullptr
    elements_ = nullptr;
  }

  // destructor
  ~FloatArray() {}

  // putting an item at the end of the array
  void append(float oo)
  {
    // allocate memory for temp array with larger size
    float* temp = new float[size_ + 1];
    // if empty array, set first element of temp array
    if (size_ == 0)
    {
      temp[0] = oo;
    }
    // else, put item to end of array
    else
    {
      for (size_t j = 0; j < size_; j++)
      {
        temp[j] = elements_[j];
      }
      temp[size_] = oo;
    }
    // set elements to the new temp array
    elements_ = temp;
    // increase size of array
    size_ += 1;
  }

  // get the n'th item in the array
  float get(size_t nn)
  {
    // if index not within bounds, return NULL
    if (size_ == 0 || nn >= size_)
    {
      return NULL;
    }
    // else return element of given index
    else
    {
      return elements_[nn];
    }
  }

  // remove the n'th item in the array
  // returning the removed item to the caller
  float remove(size_t nn)
  {
    // if size not within bounds, return NULL
    if (size_ == 0 || nn > size_)
    {
      return NULL;
    }
    else
    {
      // set removed value to be returned
      float removed = elements_[nn];
      // allocate memory for temp array with one less size since we remove
      float* temp = new float[size_ - 1];
      // copy the elements up to index to be removed in temp array
      for (int v = 0; v < nn; v++)
      {
        temp[v] = elements_[v];
      }
      // copy elements after remove index to temp array
      for (int a = nn + 1; a < size_; a++)
      {
        temp[a - 1] = elements_[a];
      }
      // set elements to new temp array
      elements_ = temp;

      // decrease array size
      size_ -= 1;
      return removed;
    }
  }

  // set n'th element to the given element
  // returns the replace Object.
  float set(size_t nn, float oo)
  {
    // if size not within bounds, return NULL
    if (size_ == 0 || nn >= size_)
    {
      return NULL;
    }
    else
    {
      float replaced = elements_[nn];
      elements_[nn] = oo;
      return replaced;
    }
  }

  // returns the length of the array.
  size_t length()
  {
    return size_;
  }

  // get the index of an float in the array
  size_t index_of(float oo)
  {
    // if empty array, return arbitrary large number
    if (size_ == 0)
    {
      return NULL;
    }
    // return a value greater than size if index not found
    else
    {
      for (size_t c = 0; c < size_; c++)
      {
        if (elements_[c] == oo)
        {
          return c;
        }
      }
      return size_ + 500;
    }
  }

  // reset and delete every element in the array
  void clear()
  {
    // if anything in array, remove it
    if (size_ > 0)
    {
      for (size_t i = 0; i < size_; i++)
      {
        remove(i);
      }
    }
    // set size to 0
    size_ = 0;
  };

  // Compares other with this array for equality.
  bool equals(Object* oo)
  {
    // set false if nullptr
    if (oo == nullptr)
    {
      return false;
    }
    // cast oo to FloatArray
    FloatArray* fa = dynamic_cast<FloatArray*>(oo);
    // set false if nullptr
    if (fa == nullptr)
    {
      return false;
    }
    // return false if length is not equal to this array
    if (fa->length() != size_)
    {
      return false;
    }
    else
    {
      // if size is 0, both arrays equal
      if (size_ == 0)
      {
        return true;
      }
      // iterate and check if all elements in both arrays are equal
      for (size_t c = 0; c < size_; c++)
      {
        if (elements_[c] != fa->elements_[c])
        {
          return false;
        }
        return true;
      }
    }
  }

  // compute by adding hash from each element
  size_t hash()
  {
    float hash = 0;
    for (size_t j = 0; j < size_; j++)
    {
      hash += elements_[j];
    }
    return hash;
  }
};

class BoolArray : public Array
{
public:
  size_t size_;
  bool* elements_;

  // constructor
  BoolArray()
  {
    size_ = 0;
    elements_ = nullptr;
  }

  // destructor
  ~BoolArray() {}

  // putting an item at the end of the array
  void append(bool oo)
  {
    // allocate memory for result array with larger size
    bool* temp = new bool[size_ + 1];
    // if empty array, set first element of result array
    if (size_ == 0)
    {
      temp[0] = oo;
    }
    // else, put item to end of array
    else
    {
      for (size_t e = 0; e < size_; e++)
      {
        temp[e] = elements_[e];
      }
      temp[size_] = oo;
    }
    // set elements to the new temp array
    elements_ = temp;
    // increase array size
    size_ += 1;
  }

  // get the n'th item in the array
  bool get(size_t nn)
  {
    // if index not within bounds, return NULL
    if (size_ == 0 || nn > size_)
    {
      return NULL;
    }
    // else return element of given index
    else
    {
      for (size_t k = 0; k < size_; k++)
      {
        if (k == nn)
        {
          return elements_[k];
        }
      }
    }
  }

  // remove the n'th item in the array
  // returning the removed item to the caller
  bool remove(size_t nn)
  {
    // if size not within bounds, return NULL
    if (size_ == 0 || nn > size_)
    {
      return NULL;
    }
    else
    {
      // set removed value to be returned
      bool removed = elements_[nn];
      // allocate memory for temp array with one less size since we remove
      bool* temp = new bool[size_ - 1];
      // copy the elements up to index to be removed in temp array
      for (size_t j = 0; j < nn; j++)
      {
        temp[j] = elements_[j];
      }
      // copy elements after remove index to temp array
      for (size_t g = nn + 1; g < size_; g++)
      {
        temp[g - 1] = elements_[g];
      }
      // set elements to new temp array
      elements_ = temp;
      // decrease size of array
      size_ -= 1;
      return removed;
    }
  }

  // set n'th element to the given element
  // returns the replace Object.
  bool set(size_t nn, bool oo)
  {
    // if size not within bounds, return NULL
    if (size_ == 0 || nn >= size_)
    {
      return NULL;
    }
    else
    {
      bool res = elements_[nn];
      elements_[nn] = oo;
      return res;
    }
  }

  // returns the length of the array.
  size_t length()
  {
    return size_;
  }

  // get the index of an element in the array
  size_t index_of(bool oo)
  {
    // if empty array, return arbitrary large number
    if (size_ == 0)
    {
      return 500;
    }
    // return a value greater than size if index not found
    else
    {
      for (size_t i = 0; i < size_; i++)
      {
        if (elements_[i] == oo)
        {
          return i;
        }
      }
      return size_ + 500;
    }
  }

  // reset and delete every element in the array
  void clear()
  {
    // if anything in array, remove it
    if (size_ > 0)
    {
      for (size_t i = 0; i < size_; i++)
      {
        remove(i);
      }
    }
    // set size to 0
    size_ = 0;
  };

  // Compares other with this array for equality.
  bool equals(Object* oo)
  {
    // set false if nullptr
    if (oo == nullptr)
    {
      return false;
    }
    // cast oo to BoolArray
    BoolArray* ba = dynamic_cast<BoolArray*>(oo);
    // set false if nullptr
    if (ba == nullptr)
    {
      return false;
    }
    // return false if length is not equal to this array
    if (ba->length() != size_)
    {
      return false;
    }
    else
    {
      // if size is 0, both arrays equal
      if (size_ == 0)
      {
        return true;
      }
      // iterate and check if all elements in both arrays are equal
      for (size_t u = 0; u < size_; u++)
      {
        if (elements_[u] != ba->elements_[u])
        {
          return false;
        }
      }
      return true;
    }
  }

  // compute by adding hash from each element
  size_t hash()
  {
    size_t hash = 0;
    for (size_t i = 0; i < size_; i++)
    {
      hash += elements_[i];
    }
    return hash;
  }
};

class StringArray : public Array
{
public:
  size_t size_;
  String** str_elements_;

  // constructor
  StringArray()
  {
    size_ = 0;
    str_elements_ = nullptr;
  }

  // destructor
  ~StringArray() {}

  // putting an item at the end of the array
  void append(String* oo)
  {
    // allocate memory for result array with larger size
    String** res = new String*[size_ + 1];
    // if not empty array, put item to end of array
    if (size_ != 0)
    {
      for (size_t i = 0; i < size_; i++)
      {
        res[i] = str_elements_[i];
      }
      res[size_] = oo;
    }
    else
    {
      // else if empty, set first element of result array
      res[0] = oo;
    }
    // set elements to the new result array
    str_elements_ = res;
    // increase size of array
    size_ += 1;
  };

  String* get(size_t nn)
  {
    // if index not within bounds, return nullptr
    if (size_ == 0 || nn >= size_)
    {
      return nullptr;
    }
    // else return element of given index
    else
    {
      for (size_t i = 0; i < size_; i++)
      {
        if (i == nn)
        {
          return str_elements_[i];
        }
      }
    }
  };

  size_t index_of(String* ss)
  {
    // if empty array, return arbitrary large number
    if (size_ == 0)
    {
      return 500;
    }
    // return a value greater than size if index not found
    else
    {
      for (size_t i = 0; i < size_; i++)
      {
        if (ss == str_elements_[i])
        {
          return i;
        }
      }
      return size_ + 500;
    };
  };

  // remove the n'th item in the array
  // returning the removed item to the caller
  String* remove(size_t nn)
  {
    // if size not within bounds, return nullptr
    if (size_ == 0 || nn >= size_)
    {
      return nullptr;
    }
    else
    {
      // make variable for removed to be returned
      String* removed;
      // allocate memory for result array with one less size since we remove
      String** res = new String*[size_ - 1];
      // copy the elements up to index to be removed in result array
      for (size_t i = 0; i < nn; i++)
      {
        res[i] = str_elements_[i];
      }
      // set removed value to be returned
      removed = str_elements_[nn];
      // copy elements after remove index to temp array
      for (size_t j = nn + 1; j < size_; j++)
      {
        res[j - 1] = str_elements_[j];
      }
      // set elements to the result array
      str_elements_ = res;
      // decrease size of array
      size_ -= 1;
      return removed;
    }
  };

  // reset and delete every element in the array
  void clear()
  {
    // if anything in array, remove it
    if (size_ > 0)
    {
      for (size_t i = 0; i < size_; i++)
      {
        remove(i);
      }
    }
    // set size to 0
    size_ = 0;
  };

  // set n'th element to the given object
  // returns the replace Object.
  String* set(size_t nn, String* oo)
  {
    // if size not within bounds, return nullptr
    if (size_ == 0 || nn >= size_)
    {
      return nullptr;
    }
    else
    {
      String* res = str_elements_[nn];
      str_elements_[nn] = oo;
      return res;
    }
  };

  // Compares other with this array for equality.
  bool equals(Object* oo)
  {
    // set false if nullptr
    if (oo == nullptr)
    {
      return false;
    }
    StringArray* s = dynamic_cast<StringArray*>(oo);
    // set false if nullptr
    if (s == nullptr)
    {
      return false;
    }
    // return false if length is not equal to this array
    if (s->length() != size_)
    {
      return false;
    }
    else
    {
      // if size is 0, both arrays equal
      if (size_ == 0)
      {
        return true;
      }
      // iterate and check if all elements in both arrays are equal
      for (size_t i = 0; i < size_; i++)
      {
        if (!str_elements_[i]->equals(s->str_elements_[i]))
        {
          return false;
        }
      }
      return true;
    }
  }

  // compute by adding hash from each element
  size_t hash()
  {
    size_t res = 0;
    for (size_t i = 0; i < size_; i++)
    {
      res += str_elements_[i]->hash_me();
    }
    return res;
  }
};