#include "object.h"
#include "string.h"
// class Key;
// class KVStore;
#include "column.h"
#include "schema.h"
#include "kvstore.h"

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
  Column **cols;

  DataFrame() {}
  /** Create a data frame with the same columns as the give df but no rows */
  DataFrame(DataFrame &df)
  {
    scm = df.scm;
    cols = new Column *[scm.width()];
    for (size_t i = 0; i < scm.width(); i++)
    {
      cols[i] = new Column();
      for (size_t j = 0; j < scm.length(); j++)
      {
        cols[i]->push_back(nullptr);
      }
    }
  }

  /** Create a data frame from a schema and columns. Results are undefined if
    * the columns do not match the schema. */
  DataFrame(Schema &schema)
  {
    scm = schema;
    cols = new Column *[scm.width()];
    for (size_t i = 0; i < scm.width(); i++)
    {
      cols[i] = createNewColumn(scm.track_types[i]);
    }
  }

  ~DataFrame()
  {
    for (size_t i = 0; i < scm.width(); i++)
      {
        delete cols[i];
      }
    delete[] cols;
  }

  void serialize(Serializer* ser) {
    scm.serialize(ser);
    for (size_t i = 0; i < ncols(); i++) {
      cols[i]->serialize(ser);
    }
  }

  DataFrame* deserialize(Deserializer* d) {
    Schema* s = scm.deserialize(d);
    cols = new Column *[scm.width()];
    for (size_t i = 0; i < s->width(); i++) {
      cols[i]->deserialize(d);
    }
    DataFrame* df = new DataFrame(*s);
    return df;
  }

  /** Returns the dataframe's schema. Modifying the schema after a dataframe
    * has been created in undefined. */
  Schema &get_schema()
  {
    return scm;
  }

  /**
   * Another helper that gets an integer depending on the type
   */
  int getTypeNum(char type)
  {
    if (type == 'I')
    {
      return 1;
    }
    if (type == 'F')
    {
      return 2;
    }
    if (type == 'B')
    {
      return 3;
    }
    if (type == 'S')
    {
      return 4;
    }
    return 0;
  }

  /**
   * Returns a new column, given its type
   * A helper to construct the dataframe
   */
  Column *createNewColumn(char type)
  {
    int typeNum = getTypeNum(type);

    switch (typeNum)
    {
    case 1:
      return new IntColumn();
    case 2:
      return new FloatColumn();
    case 3:
      return new BoolColumn();
    case 4:
      return new StringColumn();
    default:
      return new Column();
    }
  }

  /** Adds a column this dataframe, updates the schema, the new column
    * is external, and appears as the last column of the dataframe, the
    * name is optional and external. A nullptr colum is undefined. */
  void add_column(Column *col, String *name)
  {
    Column *c;
    scm.add_column(col->get_type(), name);
    switch (col->get_type())
    {
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
    Column **newCols = new Column *[scm.width()];
    for (size_t i = 0; i < scm.width() - 1; i++)
    {
      newCols[i] = cols[i];
    }
    newCols[scm.width() - 1] = c;
    cols = newCols;
  }

  /** Return the value at the given column and row. Accessing rows or
   *  columns out of bounds, or request the wrong type is undefined.*/
  int get_int(size_t col, size_t row)
  {
    if (cols[col]->get_type() != 'I' || col >= scm.width() || row >= scm.length())
    {
      printf("Index out of bounds or type incorrect!\n");
      exit(1);
    }
  }

  bool get_bool(size_t col, size_t row)
  {
    if (cols[col]->get_type() != 'B' || col >= scm.width() || row >= scm.length())
    {
      printf("Index out of bounds or type incorrect!\n");
      exit(1);
    }
  }

  double get_double(size_t col, size_t row)
  {
    if (cols[col]->get_type() != 'F' || col >= scm.width() || row >= scm.length())
    {
      printf("Index out of bounds or type incorrect!\n");
      exit(1);
    }
  }

  String *get_string(size_t col, size_t row)
  {
    if (cols[col]->get_type() != 'S' || col >= scm.width() || row >= scm.length())
    {
      printf("Index out of bounds or type incorrect!\n");
      exit(1);
    }
  }


  /** Set the value at the given column and row to the given value.
    * If the column is not  of the right type or the indices are out of
    * bound, the result is undefined. */
  void set(size_t col, size_t row, int val)
  {
    Column *c = cols[col];
    assert(c->get_type() == 'I');
    c->set(row, val);
  }

  void set(size_t col, size_t row, bool val)
  {
    Column *c = cols[col];
    assert(c->get_type() == 'B');
    c->set(row, val);
  }

  void set(size_t col, size_t row, double val)
  {
    Column *c = cols[col];
    assert(c->get_type() == 'F');
    c->set(row, (double)val);
  }

  void set(size_t col, size_t row, String *val)
  {
    Column *c = cols[col];
    assert(c->get_type() == 'S');
    c->set(row, val);
  }

  /** Set the fields of the given row object with values from the columns at
    * the given offset.  If the row is not form the same schema as the
    * dataframe, results are undefined.
    */
  void fill_row(size_t idx, Row &row)
  {
    for (size_t i = 0; i < scm.width(); i++)
    {
      if (cols[i]->get_type() == 'I')
      {
        row.set(i, cols[i]->as_int()->get(idx));
      }
      if (cols[i]->get_type() == 'F')
      {
        row.set(i, cols[i]->as_float()->get(idx));
      }
      if (cols[i]->get_type() == 'B')
      {
        row.set(i, cols[i]->as_bool()->get(idx));
      }
      if (cols[i]->get_type() == 'S')
      {
        row.set(i, cols[i]->as_string()->get(idx));
      }
    }
  }

  /** Add a row at the end of this dataframe. The row is expected to have
   *  the right schema and be filled with values, otherwise undedined.  */
  void add_row(Row &row)
  {
    scm.add_row(row.name);

    for (size_t i = 0; i < scm.width(); i++)
    {
      assert(strcmp(scm.track_types, row.track_types) == 0);
      if (cols[i]->get_type() == 'I')
      {
        cols[i]->push_back(row.get_int(i));
      }
      else if (cols[i]->get_type() == 'F')
      {
        cols[i]->push_back(row.get_float(i));
      }
      else if (cols[i]->get_type() == 'B')
      {
        cols[i]->push_back(row.get_bool(i));
      }
      else if (cols[i]->get_type() == 'S')
      {
        cols[i]->push_back(row.get_string(i));
      }

      else
      {
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
    return scm.width();
  }

  /** Visit rows in order */
  void map(Rower &r)
  {
    for (size_t i = 0; i < nrows(); i++)
    {
      Row *ro = new Row(scm);
      fill_row(i, *ro);
      r.accept(*ro);
    }
  }

  /** Create a new dataframe, constructed from rows for which the given Rower
    * returned true from its accept method. */
  DataFrame *filter(Rower &r)
  {
    Schema *s = new Schema();
    for (size_t i = 0; i < nrows(); i++)
    {
      Row *ro = new Row(*s);
      fill_row(i, *ro);

      if (r.accept(*ro))
      {
        s->add_row(nullptr);
      }
    }

    DataFrame *ndf = new DataFrame(*s);
    return ndf;
  }

    /** Print the dataframe in SoR format to standard output. */
  void print()
  {
    for (size_t i = 0; i < nrows(); i++)
    {
      for (size_t j = 0; j < ncols(); j++)
      {
        if (cols[j]->get_type() == 'I')
        {
          printf("< %i >", cols[j]->as_int()->get(i));
          printf("\t");
        }
        if (cols[j]->get_type() == 'B')
        {
          printf("< %i >", cols[j]->as_bool()->get(i));
          printf("\t");
        }
        if (cols[j]->get_type() == 'F')
          p("< ").p(cols[j]->as_float()->get(i)).p(" >\t");
        if (cols[j]->get_type() == 'S')
        {
          String *s = cols[j]->as_string()->get(i);
          printf("< %s >", s->c_str());
          printf("\t");
        }
      }
      printf("\n");
    }
  }
};
