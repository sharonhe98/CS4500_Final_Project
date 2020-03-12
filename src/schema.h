#include "object.h"
#include "array.h"
#include "column.h"
#include <string.h>
#include "string.h"

class Row;
class Fielder;
class Rower;
class Schema;
const char* get_schema_track_types(Schema &scm);
// Column** get_schema_columns(Schema &scm);

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
  virtual void accept(float f);
  virtual void accept(int i);
  virtual void accept(String* s);

  /** Called when all fields have been seen. */
  virtual void done();
};

class PrintFielder : public Fielder
{
public:
  void start(size_t r);

  void accept(bool b)
  {
    std::cout << "<" << b << ">"<< "\t";
  }

  void accept(float f)
  {
    std::cout << "<" << f << ">" << "\t";
  }

  void accept(int i)
  {
    std::cout << "<" << i << ">" << "\t";
  }

  void accept(String *s)
  {
    std::cout << "<" << s << ">" << "\t";
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
  String* name;
  const char* track_types;
  size_t index_;
  Column** columns;
  size_t width_;

  /** Build a row following a schema. */
  Row(Schema &scm)
  {
    name=nullptr;
    track_types = get_schema_track_types(scm);
    // columns = get_schema_columns(scm);
    width_ = strlen(track_types);
    printf("size init: %d\n", width_);
    columns = new Column*[width_];
    //  for (size_t i = 0; i < strlen(track_types); i++) {
	  //    add_column_to_row(track_types[i], nullptr);
    //  }
    index_ = 0;
  }

  /** Build a row given a name*/
  Row(String* s)
  {
    index_ = 0;
    columns = nullptr;
    name = s;
    width_ = 0;
    track_types = "";

  }

  // helper to add column
  void add_column_to_row(char typ, String* name) {
    Column* c = nullptr;
    if (typ == 'I') {
      c = new IntColumn;
      //c = c->as_int();
      //delete i;
    }
    if (typ == 'F') {
      FloatColumn* i = new FloatColumn();
      c = (Column*) i;
      //delete i;
    }
    if (typ == 'B') {
      BoolColumn* i = new BoolColumn();
      c = (Column*) i;
      //delete i;
    }
    if (typ == 'S') {
      StringColumn* i = new StringColumn();
      c = i ;
      //delete i;
    }

    printf("what is da width???! %d\n", width_);
    
      
      // Column** temp = new Column*[width_ + 1];
      // for (size_t i = 0; i < width_; i++) {
      //   temp[i] = columns[i];
	    //   printf("current columns %i: %d\n", i);
      // }
      size_t current_idx = 0;
      columns[current_idx] = c;
      current_idx += 1;
    
      // temp[width_] = c;
      width_ += 1;
      printf("size after add else: %d\n", width_);
      // delete [] columns;
      // delete [] temp;
      // columns = temp;
      printf("colimns is not NNULL: %i\n", columns);
    // }
    printf("hii\n");
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
    printf("col is %d\n", col);
    printf("val is %d\n", val);
    printf("int set is called\n");

    Column* c = nullptr;
    
    if (columns[col] == nullptr) {
	c = new IntColumn();
	columns[col] = c;
    }
    else {
	c = columns[col];
    }
    printf("c->type: %c\n", columns[col]->get_type());
    if (c->get_type() == 'I') {
      //IntColumn* c_i = c->as_int();
      c = c->as_int();
      printf("asInt OK - Checking set in Column\n");
      printf("index is %d\n", index_);
      
      c->set(index_, val);
      printf("set in Column OK\n");
    }
  }
  void set(size_t col, float val)
  {
    Column* c = nullptr;
    if (columns[col] == nullptr) {
	    c = new FloatColumn();
	    columns[col] = c;
    }
    else {
      c = columns[col];
    }
    if (c->get_type() == 'F') {
      c = c->as_float();
      c->set(index_,val);
    }
  }
  void set(size_t col, bool val)
  {
    Column* c = nullptr;
    if (columns[col] == nullptr) {
	    c = new BoolColumn();
	    columns[col] = c;
    }
    else {
      c = columns[col];
    }
    if (c->get_type() == 'B') {
      c = c->as_bool();
      c->set(index_,val);
    }
  }
  /** Acquire ownership of the string. */
  void set(size_t col, String* val)
  {
    Column* c = nullptr;
    if (columns[col] == nullptr) {
	    c = new StringColumn();
	    columns[col] = c;
    }
    else {
      c = columns[col];
    }
    if (c->get_type() == 'S') {
      c = c->as_string();
      c->set(index_,val);
    }
    // String* ourString = val->clone();
    // Column* c = columns[col];
    // if (c->get_type() == 'S') {
    //   StringColumn* c_s = c->as_string();
    //   c_s->vals_->set(index_, ourString);
    // }
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
    if (isRequestedType(col))
    {
      Column* c = columns[col];
      // IntColumn* c_i = c->as_int();
      IntColumn* c_i = dynamic_cast<IntColumn*>(c);
      return c_i->get(index_);
    }
    else
    {
      return NULL;
    }
  }
  bool get_bool(size_t col)
  {
    if (isRequestedType(col))
    {
      Column* c = columns[col];
      // BoolColumn* c_b = c->as_bool()
      BoolColumn* c_b = dynamic_cast<BoolColumn*>(c);
      return c_b->get(index_);
    }
    else
    {
      return NULL;
    }
  }
  float get_float(size_t col)
  {
    if (isRequestedType(col))
    {
      Column* c = columns[col];
      // FloatColumn* c_f = c->as_float();
      FloatColumn* c_f = dynamic_cast<FloatColumn*>(c);
      return c_f->get(index_);
    }
    else
    {
      return NULL;
    }
  }
  String* get_string(size_t col)
  {
    if (isRequestedType(col))
    {
      Column* c = columns[col];
      // StringColumn* c_s = c->as_string();
      StringColumn* c_s = dynamic_cast<StringColumn*>(c);
      return c_s->get(index_);
    }
    else
    {
      return NULL;
    }
  }

  /** Number of fields in the row. */
  size_t width()
  {
    printf("huhhhh???width ");
    pln(width_);
    printf("\nwidth of row in row class  \n");
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
  virtual bool accept(Row &r) {
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
  char* track_types;
  StringArray* col_names;
  StringArray* row_names;
  size_t col_size;
  size_t row_size;

  /** Copying constructor */
  Schema(Schema &from) : Object(from)
  {
    track_types = from.track_types;
    col_names = from.col_names;
    row_names = from.row_names;
    col_size = from.col_size;
    row_size = from.row_size;
  }

  /** Create an empty schema **/
  Schema()
  {
    row_names = new StringArray();
    col_names = new StringArray();
    track_types = "";
    col_size = 0;
  }

  /** Create a schema from a string of types. A string that contains
    * characters other than those identifying the four type results in
    * undefined behavior. The argument is external, a nullptr is
    * undefined. **/
  Schema(const char* types)
  {
    track_types = (char*)types;
    row_size = 0;
    col_size = 0;
    row_names = new StringArray();
    col_names = new StringArray();


    for (size_t i = 0; i < strlen(track_types); i++)
    {
      printf("before add column! %d\n", i);
      add_column(track_types[i], nullptr);
      printf("add column yay!\n");
      // add_row(nullptr);
      // printf("add row success\n");
    }
  }

  ~Schema() {
    delete [] row_names;
    delete [] col_names;
  }

  void add_column(char typ, String* name)
  {
    printf("row size: %d\n", row_size);
    printf("col size: %d\n", col_size);
    col_names->append(name);
    printf("append col success %c\n", typ);

    col_size += 1;
    printf("add_column success! col size? %i\n", col_size);
  }

  /** Add a row with a name (possibly nullptr), name is external. */
  void add_row(String* name)
  {
    row_names->append(name);
    row_size += 1;
    
  }

  /** Return name of row at idx; nullptr indicates no name. An idx >= width
    * is undefined. */
  String* row_name(size_t idx)
  {
    if (idx >= row_size) {
      printf("out of bounds\n");
      exit(1);
    }
    else {
      return row_names->get(idx);
    }
  }

  /** Return name of column at idx; nullptr indicates no name given.
    *  An idx >= width is undefined.*/
  String* col_name(size_t idx)
  {
    if (idx >= col_size) {
      printf("out of bounds\n");
      exit(1);
    }
    else {
      return col_names->get(idx);
    }

  }

  /** Return type of column at idx. An idx >= width is undefined. */
  char col_type(size_t idx)
  {
    if (idx >= width()) {
      return 'U';         // U for undefined
    }
    else {
      return track_types[idx];
    }
  };

  /** Given a column name return its index, or -1. */
  int col_idx(const char* name)
  {
    for(size_t i = 0; i < col_size; i++) {
      String* nam = col_names->get(i);
      if (strcmp(nam->c_str(), name) == 0) {
        return i;
      }
      else {
        return -1;
      }
    }
  };

  /** Given a row name return its index, or -1. */
  int row_idx(const char* name)
  {
    for(size_t i = 0; i < row_size; i++) {
      String* nam = row_names->get(i);
      if (strcmp(nam->c_str(), (char*)name) == 0) {
        return i;
      }
      else {
        return -1;
      }
    }
  }

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

const char* get_schema_track_types(Schema &scm) {
	return scm.track_types;
}
