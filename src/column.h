#pragma once

#include "array.h"
#include "helpers.h"
#include <stdarg.h>
// #include "serial.h"

// forward declaration
class IntColumn;
class StringColumn;
class FloatColumn;
class BoolColumn;

#define CHUNK_SIZE 1000

// returns the inferred typing of the char*
char inferred_type(char *c)
{
  if (c == nullptr || ((strlen(c) == 1) && ((*c == '0') || (*c == '1')))) 
  {
    return 'B';
  } 
  if (is_int(c)) {return 'I';}
  if (is_float(c)) {return 'F';}
  else {return 'S';}
}

/**************************************************************************
* Column ::
*
* Represents one column of a data frame which holds values of a single type.
* This abstract class defines methods overriden in subclasses. There is
* one subclass per element type. Columns are mutable, equality is pointer
* equality. */
class Column : public Object
{
public:
  Array **vals_;
  char type_;
  size_t currentChunk_;
  size_t currentSize_;

  Column() {}

  ~Column() {}
  /** Type converters: Return the same column under its actual type, or
 *    *  nullptr if of the wrong type.  */

  virtual IntColumn *as_int()
  {
    return nullptr;
  };
  virtual BoolColumn *as_bool()
  {
    return nullptr;
  };
  virtual FloatColumn *as_float()
  {
    return nullptr;
  };
  virtual StringColumn *as_string()
  {
    return nullptr;
  };

  /** Type appropriate push_back methods. Calling the wrong method is
 *     * undefined behavior. **/
  virtual void push_back(int val)
  {
    return;
  };
  virtual void push_back(bool val)
  {
    return;
  };
  virtual void push_back(double val)
  {
    return;
  };
  virtual void push_back(String *val)
  {
    return;
  };

  /** Returns the number of elements in the column. */
  virtual size_t size()
  {
    return vals_[currentChunk_]->length();
  };

  virtual void set(size_t idx, int val)
  {
    return;
  }
  virtual void set(size_t index_, bool val) {}
  virtual void set(size_t index_, double val) {}
  virtual void set(size_t index_, String *val) {}

  /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'.*/
  char get_type()
  {
    return type_;
  };

  // checks if the value represented by the char* can be added to this column
  virtual bool can_add(char *c)
  {
    if (c == nullptr)
    {
      return true;
    }
    return false;
  }

  virtual size_t calculateCurrentChunk(size_t idx)
  {
    size_t chunkNum = idx / CHUNK_SIZE;
    if (chunkNum > currentChunk_)
    {
      currentChunk_ = chunkNum;
    }
    return chunkNum;
  }

  char *serialize(Serializer *ser)
  {
    ser->write(type_);
    return ser->getSerChar();
  };

  static Column *deserialize(Deserializer *dser);
};

/*************************************************************************
 * IntColumn::
 * Holds primitive int values, unwrapped.
 */
class IntColumn : public Column
{
public:
  IntArray **vals_;
  size_t currentChunk_;
  size_t currentSize_;

  IntColumn()
  {
    type_ = 'I';
    vals_ = new IntArray *[CHUNK_SIZE];
    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new IntArray();
    }
    currentChunk_ = 0;
    currentSize_ = 0;
  }

  ~IntColumn()
  {
    for (size_t i = 0; i < size(); i++) {
      delete vals_[i];
    }
    delete [] vals_;
  }

  IntColumn(Deserializer *d)
  {
    type_ = d->readChar();
    currentChunk_ = d->readSizeT();
    currentSize_ = d->readSizeT();
    vals_ = new IntArray *[CHUNK_SIZE];
    for (size_t i = 0; i < currentChunk_; i++)
    {
      vals_[i]->deserializeIntArray(d);
      add_chunk(vals_[i], i);
    }
  }

  char *serialize(Serializer *ser)
  {
    Column::serialize(ser);
    ser->write(currentChunk_);
    ser->write(currentSize_);
    for (size_t i = 0; i < currentChunk_; i++)
    {
      vals_[i]->serialize(ser);
    }
    return ser->getSerChar();
  }

  void push_back(int val)
  {
    vals_[calculateCurrentChunk(currentSize_)]->append(val);
    currentSize_ += 1;
  }

  int get(size_t idx)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    return vals_[calculateCurrentChunk(idx)]->get(idx_in_val);
  }

  IntColumn *as_int()
  {
    return this;
  }

  size_t calculateCurrentChunk(size_t idx)
  {
    size_t chunkNum = idx / CHUNK_SIZE;
    if (chunkNum > currentChunk_)
    {
      currentChunk_ = chunkNum;
    }
    return chunkNum;
  }

  void add_chunk(IntArray *ia, size_t idx)
  {
    vals_[idx] = ia;
    // We assume that the arrays we add as chunks
    // will only have length 1000
    // which we arbitrarily set in the kv store
    if (ia->length() == CHUNK_SIZE)
    {
      currentChunk_ += 1;
    }
  }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, int val)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    vals_[calculateCurrentChunk(idx)]->set(idx_in_val, val);
  }

  size_t size()
  {
    return currentSize_;
  }

  bool can_add(char *c)
  {
    if (c == nullptr || inferred_type(c) == 'I' || inferred_type(c) == 'B')
    {
      return true;
    }
    return false;
  }

};

// Other primitive column classes similar...

/*************************************************************************
 * StringColumn::
 * Holds string pointers. The strings are external.  Nullptr is a valid
 * value.
 */
class StringColumn : public Column
{
public:
  StringArray **vals_;
  size_t currentChunk_;
  size_t currentSize_;

  StringColumn()
  {
    type_ = 'S';
    vals_ = new StringArray *[CHUNK_SIZE];
    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new StringArray();
    }
    currentChunk_ = 0;
    currentSize_ = 0;
  }

  StringColumn(Deserializer *d)
  {
    type_ = d->readChar();
    currentChunk_ = d->readSizeT();
    currentSize_ = d->readSizeT();
    vals_ = new StringArray *[CHUNK_SIZE];
    for (size_t i = 0; i < currentChunk_; i++)
    {
      vals_[i]->deserializeStringArray(d);
      add_chunk(vals_[i], i);
    }
  }

  void add_chunk(StringArray *sa, size_t idx)
  {
    vals_[idx] = sa;
    // We assume that the arrays we add as chunks
    // will only have length 1000
    // which we arbitrarily set in the kv store
    if (sa->length() == CHUNK_SIZE)
    {
      currentChunk_ += 1;
    }
  }

  char *serialize(Serializer *ser)
  {
    Column::serialize(ser);
    ser->write(currentChunk_);
    ser->write(currentSize_);
    for (size_t i = 0; i < currentChunk_; i++)
    {
      printf("i is: %zu vals_[i] length is: %zu\n",i, vals_[i]->length());
      vals_[i]->serialize(ser);
    }
    return ser->getSerChar();
  }

  ~StringColumn()
  {
    for (size_t i = 0; i < size(); i++) {
      delete vals_[i];
    }
    delete [] vals_;
  }

  size_t calculateCurrentChunk(size_t idx)
  {
    size_t chunkNum = idx / CHUNK_SIZE;
    if (chunkNum > currentChunk_)
    {
      currentChunk_ = chunkNum;
    }
    return chunkNum;
  }

  void push_back(String *val)
  {
    vals_[calculateCurrentChunk(currentSize_)]->append(val);
    currentSize_ += 1;
  }

  /** Returns the string at idx; undefined on invalid idx.*/
  String *get(size_t idx)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    return vals_[calculateCurrentChunk(idx)]->get(idx_in_val);
  }

  StringColumn *as_string()
  {
    return this;
  }

  /** Out of bound idx is undefined. */
  void set(size_t idx, String *val)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    vals_[calculateCurrentChunk(idx)]->set(idx_in_val, val);
  }

  size_t size()
  {
    return currentSize_;
  }

  bool can_add(char *c)
  {
    return true;
  }

  // returns the string representation of the element at the ith index
  char *get_char(size_t i)
  {
    if (i >= size() || vals_[calculateCurrentChunk(i)]->get(i) == NULL)
    {
      return nullptr;
    }
    size_t str_len = vals_[calculateCurrentChunk(i)]->get(i)->size();
    char *ret = new char[str_len + 3];
    ret[0] = '"';
    for (size_t j = 0; j < str_len; j++)
    {
      ret[j + 1] = vals_[calculateCurrentChunk(i)]->get(i)->c_str()[j];
    }
    ret[str_len + 1] = '"';
    ret[str_len + 2] = '\0';
    return ret;
  }
};

/*************************************************************************
 * FloatColumn::
 * Holds primitive float values, unwrapped.
 */
class FloatColumn : public Column
{
public:
  FloatArray **vals_;
  size_t currentChunk_;
  size_t currentSize_;

  FloatColumn()
  {
    type_ = 'F';
    vals_ = new FloatArray *[CHUNK_SIZE];
    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new FloatArray();
    }
    currentChunk_ = 0;
    currentSize_ = 0;
  }

  ~FloatColumn()
  {
    for (size_t i = 0; i < size(); i++) {
      delete vals_[i];
    }
    delete [] vals_;
  }

  FloatColumn(Deserializer *d)
  {
    type_ = d->readChar();
    currentChunk_ = d->readSizeT();
    currentSize_ = d->readSizeT();
    vals_ = new FloatArray *[CHUNK_SIZE];
    for (size_t i = 0; i < currentChunk_; i++)
    {
      vals_[i]->deserializeFloatArray(d);
      add_chunk(vals_[i], i);
    }
  }

  void add_chunk(FloatArray *fa, size_t idx)
  {
    vals_[idx] = fa;
    // We assume that the arrays we add as chunks
    // will only have length 1000
    // which we arbitrarily set in the kv store
    if (fa->length() == CHUNK_SIZE)
    {
      currentChunk_ += 1;
    }
  }

  char *serialize(Serializer *ser)
  {
    Column::serialize(ser);
    ser->write(currentChunk_);
    ser->write(currentSize_);
    for (size_t i = 0; i < currentChunk_; i++)
    {
      vals_[i]->serialize(ser);
    }
    return ser->getSerChar();
  }

  size_t calculateCurrentChunk(size_t idx)
  {

    size_t chunkNum = idx / CHUNK_SIZE;
    if (chunkNum > currentChunk_)
    {
      currentChunk_ = chunkNum;
    }

    return chunkNum;
  }

  void push_back(double val)
  {
    vals_[calculateCurrentChunk(currentSize_)]->append(val);
    currentSize_ += 1;
  }

  double get(size_t idx)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    return vals_[calculateCurrentChunk(idx)]->get(idx_in_val);
  }

  FloatColumn *as_float()
  {
    return this;
  }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, double val)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    vals_[calculateCurrentChunk(idx)]->set(idx_in_val, val);
  }

  size_t size()
  {
    return currentSize_;
  }

  bool can_add(char *c)
  {
    if (c == nullptr || inferred_type(c) == 'F' || inferred_type(c) == 'B' || inferred_type(c) == 'I')
    {
      return true;
    }
    return false;
  }

};

/*************************************************************************
 * BoolColumn::
 * Holds primitive bool values, unwrapped.
 */
class BoolColumn : public Column
{
public:
  BoolArray **vals_;
  size_t currentChunk_;
  size_t currentSize_;

  BoolColumn()
  {
    type_ = 'B';
    vals_ = new BoolArray *[CHUNK_SIZE];
    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new BoolArray();
    }
    currentChunk_ = 0;
    currentSize_ = 0;
  }

  BoolColumn(Deserializer *d)
  {
    type_ = d->readChar();
    currentChunk_ = d->readSizeT();
    currentSize_ = d->readSizeT();
    vals_ = new BoolArray *[CHUNK_SIZE];
    for (size_t i = 0; i < currentChunk_; i++)
    {
      vals_[i]->deserializeBoolArray(d);
      add_chunk(vals_[i], i);
    }
  }

  ~BoolColumn()
  {
    for (size_t i = 0; i < size(); i++) {
      delete vals_[i];
    }
    delete [] vals_;
  }

  void add_chunk(BoolArray *ba, size_t idx)
  {
    vals_[idx] = ba;
    // We assume that the arrays we add as chunks
    // will only have length 1000
    // which we arbitrarily set in the kv store
    if (ba->length() == CHUNK_SIZE)
    {
      currentChunk_ += 1;
    }
  }

  char *serialize(Serializer *ser)
  {
    Column::serialize(ser);
    ser->write(currentChunk_);
    ser->write(currentSize_);
    for (size_t i = 0; i < currentChunk_; i++)
    {
      vals_[i]->serialize(ser);
    }
    return ser->getSerChar();
  }

  void push_back(bool val)
  {
    vals_[calculateCurrentChunk(currentSize_)]->append(val);
    currentSize_ += 1;
  }

  bool get(size_t idx)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    return vals_[calculateCurrentChunk(idx)]->get(idx_in_val);
  }

  BoolColumn *as_bool()
  {
    return this;
  }

  size_t calculateCurrentChunk(size_t idx)
  {
    size_t chunkNum = idx / CHUNK_SIZE;
    if (chunkNum > currentChunk_)
    {
      currentChunk_ = chunkNum;
    }
    return chunkNum;
  }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, bool val)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    vals_[calculateCurrentChunk(idx)]->set(idx_in_val, val);
  }

  size_t size()
  {
    return currentSize_;
  }

  bool can_add(char *c)
  {
    if (c == nullptr || inferred_type(c) == 'B')
    {
      return true;
    }
    return false;
  }

};

Column *Column::deserialize(Deserializer *dser)
{
  Column *result = nullptr;
  char colType = dser->readChar();
  switch (colType)
  {
  case 'I':
    result = new IntColumn(dser);
    break;
  case 'B':
    result = new BoolColumn(dser);
    break;
  case 'F':
    result = new FloatColumn(dser);
    break;
  case 'S':
    result = new StringColumn(dser);
    break;
  }
  return result;
}
