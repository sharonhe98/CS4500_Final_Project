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
  // missing values
  if (c == nullptr)
  {
    return 'B';
  }
  // check boolean
  if (strlen(c) == 1)
  {
    if ((*c == '0') || (*c == '1'))
    {
      return 'B';
    }
  }
  // check int
  if (is_int(c))
  {
    return 'I';
  }
  // check float
  if (is_float(c))
  {
    return 'F';
  }
  return 'S';
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
  String *colName_;
  size_t currentChunk_;
  size_t currentSize_;

  Column() {}

  ~Column() {}
  /** Type converters: Return the same column under its actual type, or
 *    *  nullptr if of the wrong type.  */

  virtual void setColName(String *name)
  {
    colName_ = name;
  }

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
  virtual char get_type()
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

  // returns the string representation of the object at the ith index
  virtual char *get_char(size_t i)
  {
    return nullptr;
  }

  virtual char* serialize(Serializer *ser) {
    ser->write(type_);
  };

  virtual size_t calculateCurrentChunk(size_t idx)
  {
    if (idx == NULL)
    {
      idx = currentSize_;
    }
    size_t chunkNum = idx / CHUNK_SIZE;
    if (chunkNum > currentChunk_)
    {
      currentChunk_ = chunkNum;
    }
    return chunkNum;
  }

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
  String *colName_;
  size_t currentChunk_;
  size_t currentSize_;

  IntColumn()
  {
    type_ = 'I';
    colName_ = nullptr;
    vals_ = new IntArray*[CHUNK_SIZE];
    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new IntArray();
    }
    currentChunk_ = 0;
    currentSize_ = 0;
  }

  ~IntColumn()
  {
    delete vals_;
  }

  IntColumn(int n, ...)
  {
    type_ = 'I';
    va_list args;
    vals_ = new IntArray *[CHUNK_SIZE];
    currentChunk_ = n / CHUNK_SIZE;
    currentSize_ = 0;
    va_start(args, n);

    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new IntArray();
    }
    for (size_t i = 0; i < n; i++)
    {
      size_t chunkNum = i / CHUNK_SIZE;
      vals_[chunkNum]->append(va_arg(args, int));
    }

    va_end(args);
  }

  IntColumn(Deserializer *d)
  {
    type_ = d->readChar();
    colName_ = d->readString();
    // vals_ = vals_->deserializeIntArray(d);
  }

  char *serialize(Serializer *ser)
  {
    ser->write(type_);
    ser->write(colName_);
    // vals_->serialize(ser);
    return ser->getSerChar();
  }

  void setColName(String *name)
  {
    colName_ = name;
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
    if (idx == NULL)
    {
      idx = currentSize_;
    }
    size_t chunkNum = idx / CHUNK_SIZE;
    if (chunkNum > currentChunk_)
    {
      currentChunk_ = chunkNum;
    }
    return chunkNum;
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

  char get_type()
  {
    return type_;
  }

  bool can_add(char *c)
  {
    if (c == nullptr || inferred_type(c) == 'I' || inferred_type(c) == 'B')
    {
      return true;
    }
    return false;
  }

  // gets the string representation of the ith element
  char *get_char(size_t i)
  {
    if (i >= size() || vals_[calculateCurrentChunk(i)]->get(i) == NULL)
    {
      return nullptr;
    }
    char *ret = new char[512];
    sprintf(ret, "%i", vals_[calculateCurrentChunk(i)]->get(i));
    return ret;
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
  String *colName_;
  size_t currentChunk_;
  size_t currentSize_;

  StringColumn()
  {
    type_ = 'S';
    colName_ = nullptr;
    vals_ = new StringArray*[CHUNK_SIZE];
    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new StringArray();
    }
    currentChunk_ = 0;
    currentSize_ = 0;
  }

  StringColumn(int n, ...)
  {
    type_ = 'S';
    va_list args;
    vals_ = new StringArray *[CHUNK_SIZE];
    currentChunk_ = n / CHUNK_SIZE;
    currentSize_ = 0;
      String *valueString = new String((va_arg(args, char *)));

    va_start(args, n);

    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new StringArray();
    }
    for (size_t i = 0; i < n; i++)
    {
      size_t chunkNum = i / CHUNK_SIZE;
      vals_[chunkNum]->append(valueString);
    }

    va_end(args);
  }

  StringColumn(Deserializer *d)
  {
    type_ = d->readChar();
    colName_ = d->readString();
    // vals_ = vals_->deserializeStringArray(d);
  }

  ~StringColumn()
  {
    delete vals_;
  }

  size_t calculateCurrentChunk(size_t idx)
  {
    if (idx == NULL)
    {
      idx = currentSize_;
    }
    size_t chunkNum = idx / CHUNK_SIZE;
    if (chunkNum > currentChunk_)
    {
      currentChunk_ = chunkNum;
    }
    return chunkNum;
  }

  void setColName(String *name)
  {
    colName_ = name;
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

  char get_type()
  {
    return type_;
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
  String *colName_;
  size_t currentChunk_;
  size_t currentSize_;

  FloatColumn()
  {
    type_ = 'F';
    colName_ = nullptr;
    vals_ = new FloatArray*[CHUNK_SIZE];
    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new FloatArray();
    }
    currentChunk_ = 0;
    currentSize_ = 0;
  }

  FloatColumn(double n, ...)
  {
    type_ = 'F';
    va_list args;
    vals_ = new FloatArray *[CHUNK_SIZE];
    currentChunk_ = n / CHUNK_SIZE;
    currentSize_ = 0;

    va_start(args, n);

    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new FloatArray();
    }
    for (size_t i = 0; i < n; i++)
    {
      size_t chunkNum = i / CHUNK_SIZE;
      vals_[chunkNum]->append(va_arg(args, double));
    }

    va_end(args);
  }

  ~FloatColumn()
  {
    delete[] vals_;
  }

  size_t calculateCurrentChunk(size_t idx)
  {
    if (idx == NULL)
    {
      idx = currentSize_;
    }
    size_t chunkNum = idx / CHUNK_SIZE;
    if (chunkNum > currentChunk_)
    {
      currentChunk_ = chunkNum;
    }
    return chunkNum;
  }

  void setColName(String *name)
  {
    colName_ = name;
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

  char get_type()
  {
    return type_;
  }

  bool can_add(char *c)
  {
    if (c == nullptr || inferred_type(c) == 'F' || inferred_type(c) == 'B' || inferred_type(c) == 'I')
    {
      return true;
    }
    return false;
  }

  // get string rep of element at ith index
  char *get_char(size_t i)
  {
    if (i >= size() || vals_[calculateCurrentChunk(i)]->get(i) == NULL)
    {
      return nullptr;
    }
    char *ret = new char[512];
    sprintf(ret, "%d", vals_[calculateCurrentChunk(i)]->get(i));
    return ret;
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
  String *colName_;
  size_t currentChunk_;
  size_t currentSize_;

  BoolColumn()
  {
    type_ = 'B';
    colName_ = nullptr;
    vals_ = new BoolArray*[CHUNK_SIZE];
    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new BoolArray();
    }
    currentChunk_ = 0;
    currentSize_ = 0;
  }

  BoolColumn(bool n, ...)
  {
    type_ = 'B';
    va_list args;
    vals_ = new BoolArray *[CHUNK_SIZE];
    currentChunk_ = n / CHUNK_SIZE;
    currentSize_ = 0;

    va_start(args, n);

    for (size_t i = 0; i < CHUNK_SIZE; i++)
    {
      vals_[i] = new BoolArray();
    }
    for (size_t i = 0; i < n; i++)
    {
      size_t chunkNum = i / CHUNK_SIZE;
      vals_[chunkNum]->append(va_arg(args, int));
    }

    va_end(args);
  }

  BoolColumn(Deserializer *d)
  {
    type_ = d->readChar();
    colName_ = d->readString();
    // vals_ = vals_->deserializeBoolArray(d);
  }

  ~BoolColumn() {
    delete[] vals_;
  }

  void setColName(String *name)
  {
    colName_ = name;
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
    if (idx == NULL)
    {
      idx = currentSize_;
    }
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

  char get_type()
  {
    return type_;
  }

  bool can_add(char *c)
  {
    if (c == nullptr || inferred_type(c) == 'B')
    {
      return true;
    }
    return false;
  }

  // get str representation of ith element
  char *get_char(size_t i)
  {
    if (i >= size() || vals_[calculateCurrentChunk(i)]->get(i) == NULL)
    {
      return nullptr;
    }
    char *ret = new char[512];
    sprintf(ret, "%d", vals_[calculateCurrentChunk(i)]->get(i));
    return ret;
  }
};

Column *Column::deserialize(Deserializer *dser)
{
    Column *result = nullptr;
    char colType = dser->readChar();
    printf("colType is: %c\n", colType);
    switch (colType)
    {
    case 'I':
      result = new IntColumn(dser);
      break;
    case 'B':
      result = new BoolColumn(dser);
      break;
    // case 'F':
    //   result = new FloatColumn(dser);
    //   break;
    case 'S':
      result = new StringColumn(dser);
      break;
    }
    return result;
  }
