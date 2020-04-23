#include "../object.h"
#include "../array.h"
#include "column.h"
#include <string.h>
#include "../string.h"

class Row;
class Fielder;
class Rower;
class Schema;
const char *get_schema_track_types(Schema &scm);

/*****************************************************************************
 * Fielder::
 * A field vistor invoked by Row.
 */
class Fielder : public Object
{
public:
  /** Called before visiting a row, the argument is the row offset in the
    dataframe. */
  virtual void start(size_t r);

  /** Called for fields of the argument's type with the value of the field. */
  virtual void accept(bool b);
  virtual void accept(double f);
  virtual void accept(int i);
  virtual void accept(String *s);

  /** Called when all fields have been seen. */
  virtual void done();
};

class PrintFielder : public Fielder
{
public:
  void start(size_t r);

  void accept(bool b)
  {
    std::cout << "<" << b << ">"
              << "\t";
  }

  void accept(double f)
  {
    std::cout << "<" << f << ">"
              << "\t";
  }

  void accept(int i)
  {
    std::cout << "<" << i << ">"
              << "\t";
  }

  void accept(String *s)
  {
    std::cout << "<" << s << ">"
              << "\t";
  }

  void done();
};

/*************************************************************************
 * Row::
 *
 * This class represents a single row of data constructed according to a
 * dataframe's schema. The purpose of this class is to make it easier to add
 * read/write complete rows. Internally a dataframe hold data in columns.
 * Rows have pointer equality.
 */

class Row : public Object
{
public:
  String *name;
  const char *track_types;
  size_t index_;
  Column **columns;
  size_t width_;

  /** Build a row following a schema. */
  Row(Schema &scm)
  {
    name = nullptr;
    track_types = get_schema_track_types(scm);
    width_ = strlen(track_types);
    columns = new Column *[width_];
    for (size_t i = 0; i < strlen(track_types); i++)
    {
      if (track_types[i] == 'I')
      {
        columns[i] = new IntColumn();
      }
      if (track_types[i] == 'B')
      {
        columns[i] = new BoolColumn();
      }
      if (track_types[i] == 'F')
      {
        columns[i] = new FloatColumn();
      }
      if (track_types[i] == 'S')
      {
        columns[i] = new StringColumn();
      }
    }
    index_ = 0;
  }

  /** Build a row given a name*/
  Row(String *s)
  {
    index_ = 0;
    columns = nullptr;
    name = s;
    width_ = 0;
    track_types = "";
  }

  ~Row() {
    for (size_t i = 0; i < width(); i++) {
      delete columns[i];
    }
    delete [] columns;
  }

  // helper to add column
  void add_column_to_row(char typ, String *name)
  {
    Column *c = nullptr;
    if (typ == 'I')
    {
      c = new IntColumn;
    }
    if (typ == 'F')
    {
      FloatColumn *i = new FloatColumn();
      c = (Column *)i;
    }
    if (typ == 'B')
    {
      BoolColumn *i = new BoolColumn();
      c = (Column *)i;
    }
    if (typ == 'S')
    {
      StringColumn *i = new StringColumn();
      c = i;
    }
    size_t current_idx = 0;
    columns[current_idx] = c;
    current_idx += 1;

    width_ += 1;
  }

  // helper to check if value is in range
  bool inRange(size_t n)
  {
    if (n >= strlen(track_types) * 100)
    {
      return false;
    }
    return true;
  }

  /** Setters: set the given column with the given value. */
  void set(size_t col, int val)
  {
    Column *c = columns[col] == nullptr ? new IntColumn() : columns[col];
  
    assert(c->get_type() == 'I');
    IntColumn *ic = dynamic_cast<IntColumn *>(c);
    if (ic->size() == 0)
      ic->push_back(val);
    else
      ic->set(0, val);
  }

  void set(size_t col, double val)
  {
    Column *c = columns[col] == nullptr ? new FloatColumn() : columns[col];
  
    assert(c->get_type() == 'F');
    FloatColumn *fc = dynamic_cast<FloatColumn *>(c);
    if (fc->size() == 0)
      fc->push_back(val);
    else
      fc->set(0, val);
  }

  void set(size_t col, bool val)
  {
    Column *c = columns[col] == nullptr ? new BoolColumn() : columns[col];
  
    assert(c->get_type() == 'B');
    BoolColumn *bc = dynamic_cast<BoolColumn *>(c);
    if (bc->size() == 0)
      bc->push_back(val);
    else
      bc->set(0, val);
  }

  /** Acquire ownership of the string. */
  void set(size_t col, String *val)
  {
    Column *c = columns[col] == nullptr ? new StringColumn() : columns[col];
  
    assert(c->get_type() == 'S');
    StringColumn *sc = dynamic_cast<StringColumn *>(c);
    if (sc->size() == 0)
      sc->push_back(val);
    else
      sc->set(0, val);
    
  }

  /** Set/get the index of this row (ie. its position in the dataframe. This is
   *  only used for informational purposes, unused otherwise */
  void set_idx(size_t idx)
  {
    index_ = idx;
  }
  size_t get_idx()
  {
    return index_;
  }

  // A helper to check if a column is of the requested type
  bool isRequestedType(size_t col)
  {
    return columns[col]->get_type() == track_types[col];
  }

  /** Getters: get the value at the given column. If the column is not
    * of the requested type, the result is undefined. */
  int get_int(size_t col)
  {
    assert(isRequestedType(col));
    Column *c = columns[col];
    IntColumn *c_i = c->as_int();
    //IntColumn *c_i = dynamic_cast<IntColumn *>(c);
    return c_i->get(0);
  }
  bool get_bool(size_t col)
  {
    assert(isRequestedType(col));
    Column *c = columns[col];
    BoolColumn *c_b = dynamic_cast<BoolColumn *>(c);
    bool val = c_b->get(0);
    return val;
  }
  double get_float(size_t col)
  {
    assert(isRequestedType(col));
    Column *c = columns[col];
    FloatColumn *c_f = dynamic_cast<FloatColumn *>(c);
    return c_f->get(0);
  }
  String *get_string(size_t col)
  {
    assert(isRequestedType(col));
    Column *c = columns[col];
    StringColumn *c_s = dynamic_cast<StringColumn *>(c);
    return c_s->get(0);
  }

  /** Number of fields in the row. */
  size_t width()
  {
    return width_;
  }

  /** Type of the field at the given position. An idx >= width is
    *  undefined. */
  char col_type(size_t idx)
  {
    if (inRange(idx))
    {
      return columns[idx]->get_type();
    }
    else
    {
      // We have defined 'U' for undefined,
      // as "U" does not conflict with any of our types
      return 'U';
    }
  }

  /** Given a Fielder, visit every field of this row.
    * Calling this method before the row's fields have been set is undefined. */
  void visit(size_t idx, Fielder &f);
};

/*******************************************************************************
 *  Rower::
 *  An interface for iterating through each row of a data frame. The intent
 *  is that this class should subclassed and the accept() method be given
 *  a meaningful implementation. Rowers can be cloned for parallel execution.
 */
class Rower : public Object
{
public:
  /** This method is called once per row. The row object is on loan and
      should not be retained as it is likely going to be reused in the next
      call. The return value is used in filters to indicate that a row
      should be kept. */
  virtual bool accept(Row &r)
  {
    return true;
  }

  /** Once traversal of the data frame is complete the rowers that were
      split off will be joined.  There will be one join per split. The
      original object will be the last to be called join on. The join method
      is reponsible for cleaning up memory. */
  void join_delete(Rower *other);
};

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 */
class Schema : public Object
{
public:
  char *track_types;
  size_t col_size;
  size_t row_size;

  /** Copying constructor */
  Schema(Schema &from) : Object(from)
  {
    track_types = from.track_types;
    col_size = from.col_size;
    row_size = from.row_size;
  }

  /** Create an empty schema **/
  Schema()
  {
    track_types = nullptr;
    col_size = 0;
    row_size = 0;
  }

  Schema(Deserializer *d)
  {
    track_types = d->readChars();
    row_size = d->readSizeT();
    col_size = d->readSizeT();
  }

  /** Create a schema from a string of types. A string that contains
    * characters other than those identifying the four type results in
    * undefined behavior. The argument is external, a nullptr is
    * undefined. **/
  Schema(const char *types)
  {
    track_types = (char *)types;
    row_size = 0;
    col_size = 0;

    for (size_t i = 0; i < strlen(track_types); i++)
    {
      add_column(track_types[i], nullptr);
    }
  }

  ~Schema() {}

  void serialize(Serializer *ser)
  {
    ser->write(track_types);
    ser->write(row_size);
    ser->write(col_size);
  }

  static Schema *deserialize(Deserializer *d)
  {
    return new Schema(d);
  }

  void add_column(char typ, String *name)
  {
    col_size += 1;
  }

  /** Add a row with a name (possibly nullptr), name is external. */
  void add_row(String *name)
  {
    row_size += 1;
  }

  /** Return type of column at idx. An idx >= width is undefined. */
  char col_type(size_t idx)
  {
    if (idx >= width())
    {
      return 'U'; // U for undefined
    }
    else
    {
      return track_types[idx];
    }
  };

  /** The number of columns */
  size_t width()
  {
    return col_size;
  };

  /** The number of rows */
  size_t length()
  {
    return row_size;
  };
};

const char *get_schema_track_types(Schema &scm)
{
  return scm.track_types;
}
