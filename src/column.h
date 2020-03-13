#pragma once
#include "array.h"
#include <stdarg.h>

// forward declaration
class IntColumn;
class StringColumn;
class FloatColumn;
class BoolColumn;

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
  // Array vals_;
  Array* vals_;
  char type_;
  String* colName_;

  Column()
  {
//    colName_ = nullptr;
  //  type_ = 'U';
  }

  ~Column() {}
  /** Type converters: Return the same column under its actual type, or
 *    *  nullptr if of the wrong type.  */

  virtual void setColName(String* name) {
    colName_ = name;
  }

  virtual IntColumn* as_int()
  {
    // if (get_type() == 'I') {
    return nullptr;
    //return IntColumn* i = dynamic_cast<IntColumn*>(this);
    //   i->as_int();
    // }
    //return nullptr;
  };
  virtual BoolColumn* as_bool()
  {
    //  if (get_type() == 'B') {
    //     BoolColumn* b = (BoolColumn*)this;
    //     b->as_bool();
    //   }
    return nullptr;
    
  };
  virtual FloatColumn* as_float()
  {
    // if (get_type() == 'F') {
    //   FloatColumn* f = (FloatColumn*)this;
    //   f->as_float();
    // }
    return nullptr;
  };
  virtual StringColumn* as_string()
  {
    // if (get_type() == 'S') {
    //   StringColumn* s = (StringColumn*)this;
    //   s->as_string();
    // }
    return nullptr;
  };

  /** Type appropriate push_back methods. Calling the wrong method is
 *     * undefined behavior. **/
  virtual void push_back(int val) {
    return;
  };
  virtual void push_back(bool val) {
    return;
  };
  virtual void push_back(float val) {
    return;
  };
  virtual void push_back(String* val) {
    return;
  };

  /** Returns the number of elements in the column. */
  virtual size_t size()
  {
    vals_->length();
  };

  virtual void set(size_t idx, int val) {
        printf("desgostang\n");
	return;
  }
  virtual void set(size_t index_, bool val) {}
  virtual void set(size_t index_, float val) {}
  virtual void set(size_t index_, String* val) {}

  /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'.*/
  virtual char get_type()
  {
    return type_;
  };

  /*void* get(size_t idx) {
    return vals_->get(idx); 
  }*/

};

/*************************************************************************
 * IntColumn::
 * Holds primitive int values, unwrapped.
 */
class IntColumn : public Column
{
public:
  IntArray* vals_;
  char type_;
  String* colName_;

  IntColumn()
  {
    type_ = 'I';
    colName_ = nullptr;
    vals_ = new IntArray();
  }

  ~IntColumn(){}

  IntColumn(int n, ...)
  {
    type_ = 'I';
    va_list args;
    vals_ = new IntArray();

    va_start (args, n);

    for (int i = 0; i < n; i++) {
      printf("%d\n", va_arg(args, int));
      vals_->append(va_arg(args, int));
    }

    va_end(args);
  }

  void setColName(String* name) {
    colName_ = name;
  }

  void push_back(int val) {
    vals_->append(val);
  }

  int get(size_t idx) {
    vals_->get(idx);
  }

  IntColumn* as_int() {
    return this;
  }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, int val) {
    vals_->set(idx, val);
  }

  size_t size() {
    vals_->length();
  }

  char get_type() {
    return type_;
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
  StringArray* vals_;
  char type_;
  String* colName_;

  StringColumn() 
  {
    type_ = 'S';
    colName_ = nullptr;
    vals_ = new StringArray();
  }

  StringColumn(int n, ...) 
  {
    type_ = 'S';
    va_list args;
    vals_ = new StringArray();
    String* valueString;

    va_start (args, n);

    for (int i = 0; i < n; i++) {
      String* newString = new String((va_arg(args, char*)));
      valueString = newString;
      vals_->append(valueString);
      delete newString;
    }

    va_end(args);
  }

  ~StringColumn(){}

  void push_back(String* val) {
    vals_->append(val);
  }

  StringColumn* as_string() {
    return this;
  }
  /** Returns the string at idx; undefined on invalid idx.*/
  String* get(size_t idx) {
    return vals_->get(idx);
  }

  /** Out of bound idx is undefined. */
  void set(size_t idx, String* val) {
    vals_->set(idx, val);
  }

  size_t size() {
    vals_->length();
  }

  char get_type() {
    return type_;
  }
};

/*************************************************************************
 * FloatColumn::
 * Holds primitive float values, unwrapped.
 */
class FloatColumn : public Column
{
public:
  FloatArray* vals_;
  char type_;
  String* colName_;

  FloatColumn() 
  {
    type_ = 'F';
    colName_ = nullptr;
    vals_ = new FloatArray();
  }

  FloatColumn(float n, ...)
  {
    type_ = 'F';
    va_list args;
    vals_ = new FloatArray();

    va_start (args, n);

    for (int i = 0; i < n; i++) {
      vals_->append(va_arg(args, float));
    }

    va_end(args);
  }

  ~FloatColumn(){}

  void push_back(float val) {
    vals_->append(val);
  }

  float get(size_t idx) {
    vals_->get(idx);
  }

  FloatColumn* as_float() {
    return this;
  }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, float val) {
    vals_->set(idx, val);
  }

  size_t size() {
    return vals_->length();
  }

  char get_type() {
    return type_;
  }
};

/*************************************************************************
 * BoolColumn::
 * Holds primitive bool values, unwrapped.
 */
class BoolColumn : public Column
{
public:
  BoolArray* vals_;
  char type_;
  String* colName_;
  
  BoolColumn() 
  {
    type_ = 'B';
    colName_ = nullptr;
    vals_ = new BoolArray();
  }

  BoolColumn(bool n, ...) 
  {
    type_ = 'B';
    va_list args;
    vals_ = new BoolArray();

    va_start (args, n);

    for (int i = 0; i < n; i++) {
      vals_->append(va_arg(args, bool));
    }

    va_end(args);
  }

  ~BoolColumn(){}

  void push_back(bool val) {
    vals_->append(val);
  }

  bool get(size_t idx) {
    return vals_->get(idx);
  }

  BoolColumn* as_bool() {
    return this;
  }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, bool val) {
    vals_->set(idx, val);
  }

  size_t size() {
    return vals_->length();
  }
  
  char get_type() {
    return type_;
  }
};
