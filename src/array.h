#pragma once

#include <cstdlib>
#include "object.h"
#include "string.h"
#include "serial.h"

/**
 * Assuming an array is a java-style arraylist. Stores objects by index.
 */
class Array : public Object
{
public:
  // elements_ is a pointer to a Object* that we stored
  Object **elements_;
  // the number of items stored in the array
  size_t size_;


  // constructor
  Array()
  {
    size_ = 0;
    elements_ = nullptr;
  }

  // destructor
  ~Array() {}

  // putting an item at the end of the array
  virtual void append(Object *oo)
  {
    // allocate memory for result array with larger size
    Object **res = new Object *[size_ + 1];
    // if not empty array, put item to end of array
    if (size_ != 0) {
      for (size_t i = 0; i < size_; i++) {
        res[i] = elements_[i];
      }
      res[size_] = oo;
    } else {
      res[0] = oo;
    }
    elements_ = res;
    size_ += 1;
  }

  // get the n'th item in the array
  Object *get_(size_t nn)
  {
    assert(nn < size_);
    return elements_[nn];
  }

  // returns the length of the array.
  virtual size_t length()
  {
    return size_;
  }

  // set n'th element to the given object
  // returns the replace Object.
  virtual Object *set(size_t nn, Object *oo)
  {
    assert(size_ == 0 || nn < size_);

    Object *res = elements_[nn];
    elements_[nn] = oo;
    return res;
  }

  // get the index of an object in the array
  virtual size_t index_of(Object *oo)
  {
    // if empty array, return arbitrary large number
    if (size_ == 0)
    {
      return length() + 500;
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

  // Compares other with this array for equality.
  bool equals(Object *oo)
  {
    // set false if nullptr
    if (oo == nullptr)
    {
      return false;
    }
    // cast oo to Array
    Array *a = dynamic_cast<Array *>(oo);
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
  int *elements_;
  size_t size_;

  // constructor
  IntArray()
  {
    size_ = 0;
    elements_ = nullptr;
  }

  // destructor
  ~IntArray() {}

  // putting an item at the end of the array
  void append(int oo)
  {
    // allocate memory for result array with larger size
    int *res = new int[size_ + 1];
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
    assert(nn < size_);
    return elements_[nn];
  }

  // remove the n'th item in the array
  // returning the removed item to the caller
  int remove(size_t nn)
  {
    assert(nn < size_);
    int *temp = new int[size_ - 1];
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

  // set n'th element to the given element
  // returns the replace Object.
  int set(size_t nn, int oo)
  {
    assert(size_ == 0 || nn < size_);
    if (size_ == 0)
    {
      append(oo);
    }

    int res = elements_[nn];
    elements_[nn] = oo;
    return res;
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
  bool equals(Object *oo)
  {
    // set false if nullptr
    if (oo == nullptr)
    {
      return false;
    }
    // cast oo to IntArray
    IntArray *ia = dynamic_cast<IntArray *>(oo);
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

  char *serialize(Serializer *ser)
  {
    // serialize size of array
    ser->write(length());

    // for every string, serialize
    for (size_t i = 0; i < length(); i++)
    {
      int item = elements_[i];
      pln(item);
      ser->write(item);
      ser->getPos();
    }
    return ser->getSerChar();
  };

  IntArray *deserializeIntArray(Deserializer *d)
  {
    // new StringArray
    IntArray *deIntArray = new IntArray();

    // deserialize size at char buffer position 0
    // amount of bytes of the size field of array (8 bytes)
    size_t sizeArr = d->readSizeT();

    for (size_t i = 0; i < sizeArr; i++)
    {
      // deserialize string at the offset position
      int deserInt = d->readInt();
      // add string to result array
      deIntArray->append(deserInt);
    }

    return deIntArray;
  }
};

/**
 * Array that supports primitive type float.
 */
class FloatArray : public Array
{
public:
  // elements_ is a pointer to a int* that we stored
  double *elements_;
  // size of the array
  size_t size_;

  // constructor
  FloatArray()
  {
    // initialize array size to 0
    size_ = 0;
    // initialize elements to nullptr
    elements_ = nullptr;
  }

  // destructor
  ~FloatArray() {}

  // putting an item at the end of the array
  void append(double oo)
  {
    // allocate memory for result array with larger size
    double *res = new double[size_ + 1];
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
  double get(size_t nn)
  {
    assert(nn < size_);
    return elements_[nn];
  }

  // set n'th element to the given element
  // returns the replace Object.
  double set(size_t nn, float oo)
  {
    assert(size_ == 0 || nn < size_);
    if (size_ == 0)
    {
      append(oo);
    }

    double res = elements_[nn];
    elements_[nn] = oo;
    return res;
  }

  // return size of array
  size_t length()
  {
    return size_;
  }

  // get the index of an element in the array
  size_t index_of(double oo)
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

  // Compares other with this array for equality.
  bool equals(Object *oo)
  {
    // set false if nullptr
    if (oo == nullptr)
    {
      return false;
    }
    // cast oo to IntArray
    FloatArray *fa = dynamic_cast<FloatArray *>(oo);
    if (fa == nullptr)
    {
      return false;
    }
    // if length is not equal, return false
    if (fa->length() != size_)
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
        if (elements_[i] != fa->elements_[i])
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

  char *serialize(Serializer *ser)
  {
    // serialize size of array
    ser->write(length());
    // for every string, serialize
    for (size_t i = 0; i < length(); i++)
    {
      int item = elements_[i];
      pln(item);
      ser->write(item);
      ser->getPos();
    }
    return ser->getSerChar();
  };

  FloatArray *deserializeFloatArray(Deserializer *d)
  {
    // new StringArray
    FloatArray *deFloatArray = new FloatArray();

    // deserialize size at char buffer position 0
    // amount of bytes of the size field of array (8 bytes)
    size_t sizeArr = d->readSizeT();

    for (size_t i = 0; i < sizeArr; i++)
    {

      // deserialize string at the offset position
      double deserDouble = d->readDouble();
      // add string to result array
      deFloatArray->append(deserDouble);
    }

    return deFloatArray;
  }
};

/**
 * Array that supports primitive type bool
 */
class BoolArray : public Array
{
public:
  // elements_ is a pointer to a int* that we stored
  bool *elements_;
  // size of the array
  size_t size_;

  // constructor
  BoolArray()
  {
    // initialize array size to 0
    size_ = 0;
    // initialize elements to nullptr
    elements_ = nullptr;
  }

  // destructor
  ~BoolArray() {}

  // putting an item at the end of the array
  void append(bool oo)
  {
    // allocate memory for result array with larger size
    bool *res = new bool[size_ + 1];
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
  bool get(size_t nn)
  {
    assert(nn < size_);
    return elements_[nn];
  }

  // remove the n'th item in the array
  // returning the removed item to the caller
  bool remove(size_t nn)
  {
    // if out of bounds, return NULL
    if (size_ == 0 || nn > size_)
    {
      return NULL;
    }
    assert(nn < size_);
    // allocate memory for temp array with one less than size
    bool *temp = new bool[size_ - 1];
    // set the result to be returned to the element at given index
    bool res = elements_[nn];
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

  // set n'th element to the given element
  // returns the replace Object.
  bool set(size_t nn, bool oo)
  {
    assert(size_ == 0 || nn < size_);
    if (size_ == 0)
    {
      append(oo);
    }

    bool res = elements_[nn];
    elements_[nn] = oo;
    return res;
  }

  // return size of array
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
  bool equals(Object *oo)
  {
    // set false if nullptr
    if (oo == nullptr)
    {
      return false;
    }
    // cast oo to IntArray
    BoolArray *ba = dynamic_cast<BoolArray *>(oo);
    if (ba == nullptr)
    {
      return false;
    }
    // if length is not equal, return false
    if (ba->length() != size_)
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
        if (elements_[i] != ba->elements_[i])
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

  char *serialize(Serializer *ser)
  {
    // serialize size of array
    ser->write(length());
    // for every string, serialize
    for (size_t i = 0; i < length(); i++)
    {
      bool item = elements_[i];
      pln(item);
      ser->write(item);
      ser->getPos();
    }
    return ser->getSerChar();
  };

  BoolArray *deserializeBoolArray(Deserializer *d)
  {
    // new StringArray
    BoolArray *deBoolArray = new BoolArray();

    // deserialize size at char buffer position 0
    // amount of bytes of the size field of array (8 bytes)
    size_t sizeArr = d->readSizeT();

    for (size_t i = 0; i < sizeArr; i++)
    {
      // deserialize string at the offset position
      bool deserBool = d->readBool();
      // add string to result array
      deBoolArray->append(deserBool);
    }

    return deBoolArray;
  }
};

/**
 * Array that supports String
 */
class StringArray : public Array
{
public:
  //String **elements_;

  String *get(size_t nn)
  {
    return dynamic_cast<String *>(get_(nn));
  }

  char *serialize(Serializer *ser)
  {
    // serialize size of array
    ser->write(length());
    // for every string, serialize
    for (size_t i = 0; i < length(); i++)
    {
      String *str = dynamic_cast<String *>(get_(i));
      ser->write(str);
    }
    return ser->getSerChar();
  };

  StringArray *deserializeStringArray(Deserializer *d)
  {
    // new StringArray
    StringArray *deStrArray = new StringArray();

    // deserialize size at char buffer position 0
    // amount of bytes of the size field of array (8 bytes)
    size_t sizeArr = d->readSizeT();

    for (size_t i = 0; i < sizeArr; i++)
    {
      // deserialize string at the offset position
      String *deserStr = d->readString();
      // add string to result array
      deStrArray->append(deserStr);
    }

    return deStrArray;
  }
};
