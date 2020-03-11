#include "object.h"
#include "string.h"
#include "column.h"
#include "schema.h"
/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A dataframe has a schema that
 * describes it.
 */
class DataFrame : public Object
{
public:
  Schema scm;
  Column** cols;

  /** Create a data frame with the same columns as the give df but no rows */
  DataFrame(DataFrame &df)
  {
    scm = df.scm;
    cols = new Column*[scm.width()];
    for (size_t i = 0; i < scm.width(); i++) {
	    cols[i] = new Column();
	    cols[i]->setColName(scm.col_names->get(i));
	    for (size_t j = 0; j < scm.length(); j++) {
		    cols[i]->push_back(nullptr);
	    }
    }
  }

  /** Create a data frame from a schema and columns. Results are undefined if
    * the columns do not match the schema. */
  DataFrame(Schema &schema)
  {
    scm = schema;
    cols = new Column*[scm.width()];
    for (size_t i = 0; i < scm.width(); i++) {
	    cols[i] = new Column();
	    cols[i]->setColName(scm.col_names->get(i));
	    for (size_t j = 0; j < scm.length(); j++) {
		    cols[i]->push_back(nullptr);
	    }
    }

  }

  ~DataFrame() {
	delete[] cols;
  }

  /** Returns the dataframe's schema. Modifying the schema after a dataframe
    * has been created in undefined. */
  Schema &get_schema()
  {
    return scm;
  }

  /** Adds a column this dataframe, updates the schema, the new column
    * is external, and appears as the last column of the dataframe, the
    * name is optional and external. A nullptr colum is undefined. */
  void add_column(Column* col, String* name)
  {
    Column* c;
    scm.add_column(col->get_type(), name);
    switch(col->get_type()) {
	case 'I':
		c = new IntColumn();
		break;
	case 'F':
		c = new FloatColumn();
		break;
	case 'B':
		c = new BoolColumn();
		break;
	case 'S':
		c = new StringColumn();
		break;
	default:
		c = new Column();	
    }
    c->setColName(name);
    Column** newCols = new Column*[scm.width()];
    for (size_t i = 0; i < scm.width() - 1; i++) {
	    newCols[i] = cols[i];
    }
    newCols[scm.width() - 1] = c;
    cols = newCols; 
  }

  /** Return the value at the given column and row. Accessing rows or
   *  columns out of bounds, or request the wrong type is undefined.*/
  int get_int(size_t col, size_t row)
  {
    if (cols[col]->get_type() != 'I' || col >= scm.width() || row >= scm.length()) {
	printf("Index out of bounds or type incorrect!\n");
	exit(1);
   }
  }
  bool get_bool(size_t col, size_t row)
  {
   if (cols[col]->get_type() != 'B' || col >= scm.width() || row >= scm.length()) {
	printf("Index out of bounds or type incorrect!\n");
	exit(1);
   }

  }
  float get_float(size_t col, size_t row)
  {
   if (cols[col]->get_type() != 'I' || col >= scm.width() || row >= scm.length()) {
	printf("Index out of bounds or type incorrect!\n");
	exit(1);
   }

  }
  String* get_string(size_t col, size_t row)
  {
   if (cols[col]->get_type() != 'I' || col >= scm.width() || row >= scm.length()) {
	printf("Index out of bounds or type incorrect!\n");
	exit(1);
   }

  }

  /** Return the offset of the given column name or -1 if no such col. */
  int get_col(String &col)
  {
    for (size_t i = 0; i < scm.width(); i++) {
	if (scm.col_names->get(i)->equals(&col)) {
		return i;
	}
    }
  }

  /** Return the offset of the given row name or -1 if no such row. */
  int get_row(String &col)
  {
    for (size_t i = 0; i < scm.length(); i++) {
	if (scm.row_names->get(i)->equals(&col)) {
		return i;
	}
    }

  }

  /** Set the value at the given column and row to the given value.
    * If the column is not  of the right type or the indices are out of
    * bound, the result is undefined. */
  void set(size_t col, size_t row, int val)
  {
    printf("DATAFRAME SET CALLED\n");
    Column* c = cols[col];
    if (c->get_type() == 'I')
    {
      c->set(row, val);
    }
    else {
      printf("Type is not of I\n");
      exit(1);
    }
  }
  void set(size_t col, size_t row, bool val)
  {
    Column* c = cols[col];
    if (c->get_type() == 'B')
    {
      c->set(row, val);
    }
    else {
      printf("Type is not of B\n");
      exit(1);
    }

  }
  void set(size_t col, size_t row, float val)
  {
    Column* c = cols[col];
    if (c->get_type() == 'F')
    {
      c->set(row, val);
    }
    else {
      printf("Type is not of F\n");
      exit(1);
    }
  }
  void set(size_t col, size_t row, String* val)
  {
    Column* c = cols[col];
    if (c->get_type() == 'S')
    {
      c->set(row, val);
    }
    else {
      printf("Type is not of S\n");
      exit(1);
    }

  }

  /** Set the fields of the given row object with values from the columns at
    * the given offset.  If the row is not form the same schema as the
    * dataframe, results are undefined.
    */
  void fill_row(size_t idx, Row &row)
  {
    printf("wjjjj!!!!\n");
    //pln(row.width());
    // TODO: check undefined later
    //
    //for (size_t i = 0; i < scm.width(); i++)
    //{
    printf("index rn is: %d\n", idx);
    printf("size of scm.row\n");
//    Row* r = scm.row[idx];
//    pln(sizeof(scm.row));
    printf("size of scm.col_size\n");
//    pln(scm.col_size);
    for (size_t i = 0; i < scm.width(); i++) {
      printf("huhhh\n");
      printf("\ni is: \n");
     // pln(i);
      printf("eeee2\n");
      //pln(r->columns[i]->type_);

      row.set(i, cols[i]->vals_->get(idx));
    }
  }

  /** Add a row at the end of this dataframe. The row is expected to have
   *  the right schema and be filled with values, otherwise undedined.  */
  void add_row(Row &row)
  {
    printf("row name\n");
    scm.add_row(row.name);
    for (size_t i = 0; i < scm.width(); i++) {
	if (row.col_type(i) != cols[i]->get_type()) {
		printf("Incorrect schema!\n");
		exit(1);
	}
	else if (cols[i]->get_type() == 'I') {
		cols[i]->push_back(row.get_int(i));
	}
	else if (cols[i]->get_type() == 'F') {
		cols[i]->push_back(row.get_float(i));
	}
	else if (cols[i]->get_type() == 'B') {
		cols[i]->push_back(row.get_bool(i));
	}
	else if (cols[i]->get_type() == 'S') {
		cols[i]->push_back(row.get_string(i));
	}

	else {
		printf("Undefined col type\n");
		exit(1);
	}
    }
  }

  /** The number of rows in the dataframe. */
  size_t nrows()
  {
    return scm.length();
  }

  /** The number of columns in the dataframe.*/
  size_t ncols()
  {
    printf("hi from dataframe ncols\n");
    return scm.width();
  }

  /** Visit rows in order */
  void map(Rower &r) {
    for (size_t i = 0; i < nrows(); i++) {
      Row* ro = new Row(scm.row_names->get(i));
      fill_row(i, *ro);
      r.accept(*ro);
    }
  }

  /** Create a new dataframe, constructed from rows for which the given Rower
    * returned true from its accept method. */
  DataFrame *filter(Rower &r) {
    Schema* s = new Schema();
    for (size_t i = 0; i < nrows(); i++) {
      Row* ro = new Row(scm.row_names->get(i));
      fill_row(i, *ro);

      if (r.accept(*ro)) {
        s->add_row(scm.row_names->get(i));
      }
    }
    
    DataFrame* ndf = new DataFrame(*s);
    return ndf;
  }

  /** Print the dataframe in SoR format to standard output. */
  void print();
};
