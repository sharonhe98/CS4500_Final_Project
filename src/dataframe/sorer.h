//lang:CwC
#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../object.h"
#include "dataframe.h"
#include "helpers.h"

// The maximum length of a line buffer. No lines over 4095 bytes
static const int buff_len = 4096;

enum ColumnType
{
    type_unknown = -1,
    type_bool = 0,
    type_int = 1,
    type_float = 2,
    type_string = 3,
};

// Reads a file and determines the schema on read
class SOR : public Object
{
public:
    Column **cols_; // owned
    size_t len_;
    size_t cap_;

    SOR()
    {
        len_ = 0;
        cap_ = 16;
        cols_ = new Column *[cap_];

        for (size_t i = 0; i < cap_; i++)
        {
            cols_[i] = nullptr;
        }
    }

    ~SOR()
    {
        for (size_t i = 0; i < len_; i++)
        {
            if (cols_[i] != nullptr)
            {
                delete cols_[i];
            }
        }
        delete[] cols_;
    }

    // What is the type of the column at the given index? i
    // if the index is too big a -1 is returned
    char get_col_type(size_t index)
    {
        if (index >= len_)
        {
            return 'U';
        }
        return cols_[index]->get_type();
    }

    // reallocates columns array if more space is needed.
    void check_reallocate_()
    {
        if (len_ >= cap_)
        {
            cap_ *= 2;
            Column **temp = new Column *[cap_];
            for (size_t i = 0; i < len_; i++)
            {
                temp[i] = cols_[i];
            }
            delete[] cols_;
            cols_ = temp;
        }
    }

    // moves the file pointer to the start of the next line.
    void seek_(FILE *f, size_t from)
    {
        if (from == 0)
        {
            fseek(f, from, SEEK_SET);
        }
        else
        {
            char buf[buff_len];
            fseek(f, from - 1, SEEK_SET);
            fgets(buf, buff_len, f);
        }
    }

    // Helper to convert column type to an enum rank

    ColumnType typeToEnum(char typ)
    {
        if (typ == 'B')
        {
            return type_bool;
        }
        if (typ == 'I')
        {
            return type_int;
        }
        if (typ == 'F')
        {
            return type_float;
        }
        if (typ == 'S')
        {
            return type_string;
        }
        else
        {
            return type_unknown;
        }
    }

    // infers and creates the column objects
    void infer_columns_(FILE *f, size_t from, size_t len)
    {
        seek_(f, from);
        char buf[buff_len];

        size_t total_bytes = 0;
        size_t row_count = 0;
        while (fgets(buf, buff_len, f) != nullptr && row_count < 500)
        {
            row_count++;
            total_bytes += strlen(buf);
            if (total_bytes >= len)
            {
                break;
            }
            size_t num_fields;
            char **row = parse_row_(buf, &num_fields);

            for (size_t i = 0; i < num_fields; i++)
            {
                check_reallocate_();
                if (i >= len_)
                {
                    cols_[i] = new BoolColumn();
                    len_++;
                }
                char type = inferred_type(row[i]);
                ColumnType colType = typeToEnum(type);
                if (colType > typeToEnum(cols_[i]->get_type()))
                {
                    delete cols_[i];
                    switch (colType)
                    {
                    case type_bool:
                        cols_[i] = new BoolColumn();
                        break;
                    case type_int:
                        cols_[i] = new IntColumn();
                        break;
                    case type_float:
                        cols_[i] = new FloatColumn();
                        break;
                    default:
                        cols_[i] = new StringColumn();
                        break;
                    }
                }
            }
            delete[] row;
        }
    }

    // added by us,
    // set df values
    DataFrame *setDataFrame(FILE *f, size_t from, size_t len)
    {
        Schema s(getSchema(f, from, len));
        DataFrame *df = new DataFrame(s);
        seek_(f, from);
        char buf[buff_len];

        size_t total_bytes = 0;
        while (fgets(buf, buff_len, f) != nullptr)
        {
            total_bytes += strlen(buf);
            if (total_bytes >= len)
            {
                break;
            }
            Row r(s);
            size_t num_fields;
            char **row = parse_row_(buf, &num_fields);
            for (size_t i = 0; i < df->ncols(); i++)
            {
                if (i >= num_fields)
                {
                    if (cols_[i]->get_type() == 'I')
                    {
                        r.set(i, (int)NULL);
                    }
                    if (cols_[i]->get_type() == 'F')
                    {
                        r.set(i, (float)NULL);
                    }
                    if (cols_[i]->get_type() == 'B')
                    {
                        r.set(i, (bool)NULL);
                    }
                    else
                    {
                        String *mt = new String("");
                        r.set(i, mt);
                    }
                }

                else if (cols_[i]->get_type() == 'B')
                {
                    bool b = (bool)atoi(row[i]);
                    r.set(i, b);
                }
                else if (cols_[i]->get_type() == 'I')
                {
                    int in = atoi(row[i]);
                    r.set(i, in);
                }
                else if (cols_[i]->get_type() == 'F')
                {
                    float f = atof(row[i]);
                    r.set(i, f);
                }
                else
                {
                    String *str = new String(row[i]);
                    r.set(i, str);
                }
            }
            df->add_row(r);
        }
        return df;
    }

    //added by us after the fact
    //converts from ColumnType to String*
    String *typeConvertToString(char type)
    {
        String * tmp;
        if (type == 'B')
        {
            tmp = new String("B");
        }
        if (type == 'I')
        {
            tmp = new String("I");
        }
        if (type == 'F')
        {
            tmp = new String("F");
        }
        if (type == 'S')
        {
            tmp = new String("S");
        }
        if (type == 'U')
        {
            tmp = new String("U");
        }

        return tmp;
    }

    // method added by us, separate from original implementation
    char *getSchema(FILE *f, size_t from, size_t len)
    {
        infer_columns_(f, from, len);
        String *schemaString = new String("");
        for (size_t i = 0; i < len_; i++)
        {
            char colType = cols_[i]->get_type();
            String *typConvert = typeConvertToString(colType);
            String * res = schemaString->concat(typConvert);
            delete schemaString;
            schemaString = res;

        }
        return schemaString->c_str();
    }

    // Find the start of the field value and null terminate it.
    // ASSUMPTION: input field is terminated by '>' char
    // NOTE: will mutate the field value
    // The value of len will be the offset of the null byte
    char *parse_field_(char *field, int *len)
    {
        char *ret = field;
        int j = 0;
        for (size_t i = 0; field[i] != '>'; i++)
        {
            switch (field[i])
            {
            case '<': // Malformed input
                affirm(false, "Multiple opening <");
            case ' ': // extra space in front of field
                ret++;
                break;
            case '"': // the start of a String
                ret++;
                j = i + 1;
                while (field[j] != '"')
                { // add every character until a end quote is met.
                    j++;
                }
                field[j] = '\0';
                *len = j;
                return ret;
            default: // add every ASCII character to field return value.
                for (j = i; field[j] != '>' && field[j] != ' '; j++)
                    ;
                field[j] = '\0';
                *len = j;
                return ret;
            }
        }
        *len = 0;
        return nullptr; // missing value
    }

    // parses a row and returns a list of field values as char*
    // NOTE: will mutate the row value.
    // The value of len will be the number of fields returned
    char **parse_row_(char *row, size_t *len)
    {
        int cap = 16;
        int l = 0;
        char **output = new char *[cap];

        for (size_t i = 0; row[i] != '\0'; i++)
        {
            if (row[i] == '<')
            {
                int to_increment = 0;
                if (l >= cap)
                {
                    cap *= 2;
                    char **temp = new char *[cap];
                    for (int k = 0; k < l; k++)
                    {
                        temp[k] = output[k];
                    }
                    delete[] output;
                    output = temp;
                }

                output[l++] = parse_field_(&row[i + 1], &to_increment);
                i = i + to_increment + 1;
            }
        }

        *len = l;
        return output;
    }
};