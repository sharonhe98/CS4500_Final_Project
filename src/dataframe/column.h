#pragma once

#include "../array.h"
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

  Column() {
    vals_ = new Array*[CHUNK_SIZE];
  }
  Column(Deserializer* d, char typ) {
    type_ = typ;
  }

  ~Column() {
    delete [] vals_;
  }
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
  virtual size_t size() {return 0;};

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

  virtual void serialize(Serializer *ser) {}

  static Column *deserialize(Deserializer *dser);
};

/*************************************************************************
 * IntColumn::
 * Holds primitive int values, unwrapped.
 */
class ArrayIntArray : public Array {
  public:

  ArrayIntArray() {}

  static void serialize(Serializer* s ) {}

  IntArray* get(size_t i) {
    return dynamic_cast<IntArray *>(Array::get_(i));
  }
};

class IntColumn : public Column
{
public:
  ArrayIntArray chunks_;
  size_t size_ = 0;
  IntArray * newChunk = new IntArray();

  IntColumn()
  {
    type_ = 'I';
    chunks_.append(newChunk);
  }

  ~IntColumn() {
    for (size_t i = 0; i < chunks_.length(); i++) {
      delete chunks_.get(i);
    }
    delete newChunk;
  }

  IntColumn(Deserializer *d) : Column(d, 'I')
  {
    size_t numStr = d->readSizeT();
    for (size_t i = 0; i < numStr; i++) {
      int ii = d->readInt();
      push_back(ii);
    }
    size_ = numStr;
  }

  void serialize(Serializer *ser)
  {
    ser->write(type_);
    ser->write(size_);
    for (size_t i = 0; i < size_; i++)
    {
     ser->write(get(i));
    }
  }



  void push_back(int val)
  {
    size_t chunk_id = size_ / CHUNK_SIZE;
    if (chunk_id >= chunks_.length()) {
      IntArray * arr = new IntArray();
      chunks_.append(arr);
    }
    chunks_.get(chunk_id)->append(val);
    size_++;
  }

  int get(size_t idx)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    return chunks_.get(idx/CHUNK_SIZE)->get(idx_in_val);
  }

  IntColumn *as_int()
  {
    return this;
  }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, int val)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    chunks_.get(idx/CHUNK_SIZE)->set(idx_in_val, val);
  }

  size_t size()
  {
    return size_;
  }


};

class ArrayStringArray : public Array {
  public:

  ArrayStringArray() {}

  static void serialize(Serializer* s ) {}

  StringArray* get(size_t i) {
    return dynamic_cast<StringArray *>(Array::get_(i));
  }
};

// Other primitive column classes similar...
/*************************************************************************
 * StringColumn::
 * Holds string pointers. The strings are external.  Nullptr is a valid
 * value.
 */
class StringColumn : public Column {
  public:
  ArrayStringArray chunks_;
  size_t size_ = 0;
  StringArray * newChunk = new StringArray();

  StringColumn() {
    type_ = 'S';
    chunks_.append(newChunk);
  }

  StringColumn(Deserializer *d) : Column(d, 'S')
  {
    size_t numStr = d->readSizeT();
    for (size_t i = 0; i < numStr; i++) {
      push_back(d->readString());
    }
    size_ = numStr;
  }

  ~StringColumn() {
    for (size_t i = 0; i < chunks_.length(); i++) {
      delete chunks_.get(i);
    }
    delete newChunk;
  }

  String *get(size_t idx)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    return chunks_.get(idx/CHUNK_SIZE)->get(idx_in_val);
  }

  void push_back(String *val)
  {
    size_t chunk_id = size_ / CHUNK_SIZE;
    if (chunk_id >= chunks_.length()) {
      StringArray * arr = new StringArray();
      chunks_.append(arr);
    }
    chunks_.get(chunk_id)->append(val);
    size_++;
  }

  void serialize(Serializer* s) {
    s->write(type_);
    s->write(size_);
    for (size_t i = 0; i < size_; i++) {
      s->write(get(i));
    }
  }

  StringColumn *as_string()
  {
    return this;
  }

  void set(size_t idx, String * val)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    chunks_.get(idx/CHUNK_SIZE)->set(idx_in_val, val);
  }

  size_t size()
  {
    return size_;
  }

};


/*************************************************************************
 * FloatColumn::
 * Holds primitive float values, unwrapped.
 */
class ArrayFloatArray : public Array {
  public:

  ArrayFloatArray() {}

  static void serialize(Serializer* s ) {}

  FloatArray* get(size_t i) {
    return dynamic_cast<FloatArray *>(Array::get_(i));
  }
};


class FloatColumn : public Column
{
public:
  ArrayFloatArray chunks_;
  size_t size_ = 0;
  FloatArray * newChunk = new FloatArray();

  FloatColumn()
  {
    type_ = 'F';
    chunks_.append(newChunk);
  }

  ~FloatColumn() {
    for (size_t i = 0; i < chunks_.length(); i++) {
      delete chunks_.get(i);
    }
    delete newChunk;
  }

  FloatColumn(Deserializer *d) :  Column(d, 'F')
  {
    size_t numStr = d->readSizeT();
    for (size_t i = 0; i < numStr; i++) {
      push_back(d->readDouble());
    }
    size_ = numStr;
  }

  void serialize(Serializer *ser)
  {
    ser->write(type_);
    ser->write(size_);
    for (size_t i = 0; i < size_; i++)
    {
     ser->write(get(i));
    }
  }

  void push_back(double val)
  {
    size_t chunk_id = size_ / CHUNK_SIZE;
    if (chunk_id >= chunks_.length()) {
      FloatArray * arr = new FloatArray();
      chunks_.append(arr);
    }
    chunks_.get(chunk_id)->append(val);
    size_++;
  }

  double get(size_t idx)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    return chunks_.get(idx/CHUNK_SIZE)->get(idx_in_val);
  }

  FloatColumn *as_float()
  {
    return this;
  }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, double val)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    chunks_.get(idx/CHUNK_SIZE)->set(idx_in_val, val);
  }

  size_t size()
  {
    return size_;
  }

};

/*************************************************************************
 * BoolColumn::
 * Holds primitive bool values, unwrapped.
 */
class ArrayBoolArray : public Array {
  public:

  ArrayBoolArray() {}

  static void serialize(Serializer* s ) {}

  BoolArray* get(size_t i) {
    return dynamic_cast<BoolArray *>(Array::get_(i));
  }
};

class BoolColumn : public Column
{
public:
  ArrayBoolArray chunks_;
  size_t size_ = 0;
  BoolArray * newChunk = new BoolArray();

  BoolColumn()
  {
    type_ = 'B';
    chunks_.append(newChunk);
  }

  BoolColumn(Deserializer *d) : Column(d, 'B')
  {
    size_t numStr = d->readSizeT();
    for (size_t i = 0; i < numStr; i++) {
      bool b = d->readBool();
      push_back(b);
    }
    size_ = numStr;
  }

  ~BoolColumn() {
    for (size_t i = 0; i < chunks_.length(); i++) {
      delete chunks_.get(i);
    }
    delete newChunk;
  }

  void serialize(Serializer *ser)
  {
    ser->write(type_);
    ser->write(size_);
    for (size_t i = 0; i < size_; i++)
    {
     ser->write(get(i));
    }
  }

  void push_back(bool val)
  {
    size_t chunk_id = size_ / CHUNK_SIZE;
    if (chunk_id >= chunks_.length()) {
      BoolArray * arr = new BoolArray();
      chunks_.append(arr);
    }
    chunks_.get(chunk_id)->append(val);
    size_++;
  }

  bool get(size_t idx)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    return chunks_.get(idx/CHUNK_SIZE)->get(idx_in_val);
  }

  BoolColumn *as_bool()
  {
    return this;
  }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, bool val)
  {
    size_t idx_in_val = idx % CHUNK_SIZE;
    chunks_.get(idx/CHUNK_SIZE)->set(idx_in_val, val);
  }

  size_t size()
  {
    return size_;
  }

};

Column *Column::deserialize(Deserializer *dser)
{
  Column *result = nullptr;
  char typ = dser->readChar();
  switch (typ)
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
