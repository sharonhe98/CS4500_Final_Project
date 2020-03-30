#include <unistd.h>
#include "string.h"
#include <string.h>
#include <assert.h>
#include "../src/helper.h"
#include "../src/array.h"
#include "../src/schema.h"

#define LOG(...) fprintf(stderr, "(" __FILE__ ") " __VA_ARGS__);

void testStringSerialize() {
    Serializer serializer;
    String * strfoo = new String("foo");
    serializer.write(strfoo);

    char* result = serializer.getSerChar();
    Deserializer* deserializer = new Deserializer(result);
    String* s = deserializer->readString();
    assert(strfoo->equals(s));
    delete strfoo;
    delete deserializer;
    printf("String serialize passed!\n");
}

void testStringArrSerialize() {
    Serializer* serializer = new Serializer();
    StringArray *sa = new StringArray();
    String * strfoo = new String("foo");
    String * strbar = new String("bar");

    assert(sa->length() == 0);
    sa->append(strfoo);
    assert(sa->length() == 1);
    sa->append(strbar);
    assert(sa->length() == 2);
    printf("before serialize strArray\n");
    char* result = sa->serialize(serializer);
    printf("after serialized\n");
    Deserializer* deserializer = new Deserializer(result);
    StringArray * dsa = sa->deserializeStringArray(deserializer);
    assert(dsa->length() == 2);
    String* s1 = dsa->get(0);
    assert(s1->equals(sa->get(0)));
    String* s2 = dsa->get(1);
    assert(s2->equals(sa->get(1)));
    delete serializer;
    delete[] sa;
    delete strfoo;
    delete strbar;
    delete deserializer;
    printf("test StringArray de/serialize passed!\n");
}

void testIntArrSerialize() {
    Serializer* serializer = new Serializer();
    IntArray *ia = new IntArray();

    assert(ia->length() == 0);
    ia->append(1);
    assert(ia->length() == 1);
    ia->append(2);
    assert(ia->length() == 2);
    ia->append(3);
    assert(ia->length() == 3);
    char* result = ia->serialize(serializer);
    Deserializer* deserializer = new Deserializer(result);
    IntArray * dia = ia->deserializeIntArray(deserializer);
    assert(dia->length() == 3);
    int i1 = dia->get(0);
    assert(i1 == ia->get(0));
    int i2 = dia->get(1);
    assert(i2 == ia->get(1));
    int i3 = dia->get(2);
    assert(i3 == ia->get(2));
    delete serializer;
    delete[] ia;
    delete deserializer;
    printf("test IntArray de/serialize passed!\n");
}

void testBoolArrSerialize() {
    Serializer* serializer = new Serializer();
    BoolArray *ba = new BoolArray();

    assert(ba->length() == 0);
    ba->append(1);
    assert(ba->length() == 1);
    ba->append(0);
    assert(ba->length() == 2);
    ba->append(1);
    assert(ba->length() == 3);
    printf("before serialize boolArray\n");
    char* result = ba->serialize(serializer);
    printf("after serialized boolArray\n");
    Deserializer* deserializer = new Deserializer(result);
    BoolArray * dba = ba->deserializeBoolArray(deserializer);
    assert(dba->length() == 3);
    bool b1 = dba->get(0);
    printf("bool is %d\n", b1);
    assert(b1 == ba->get(0));
    bool b2 = dba->get(1);
    printf("bool is %d\n", b2);
    assert(b2 == ba->get(1));
    bool b3 = dba->get(2);
    printf("bool is %d\n", b3);
    assert(b3 == ba->get(2));
    delete serializer;
    delete ba;
    delete deserializer;
    printf("test BoolArray de/serialize passed!\n");
}

void testCharStar() {
    Serializer* serializer = new Serializer();
    char* types = "IFSB";
    serializer->write(types);
    char* result = serializer->getSerChar();
    Deserializer* deserializer = new Deserializer(result);
    char* deChar = deserializer->readChars();
    assert(types == deChar);
    assert(strlen(types) == strlen(deChar));
    assert(types[1] == deChar[1]);
    printf("deChar result: %s\n", deChar);
    delete serializer;
    delete deserializer;
    printf("char* deserialized!\n");
    printf("huh?\n");
}

void testSchemaSerialize() {
    printf("Called?\n");
    Serializer* serializer = new Serializer();
    Schema* isfb = new Schema("ISFB");
    printf("schema is built\n");
    isfb->serialize(serializer);
    Deserializer* deserializer;
    printf("serialize failed?\n");
    Schema* deSchema = Schema::deserialize(deserializer);
    printf("test failed?\n");
    assert(isfb->track_types == deSchema->track_types);
    printf("Schema serialize success!\n");
}

// void testColumnSerialize() {
//     Serializer* serializer = new Serializer();
//     Column()
// }

int main(int argc, char **argv) {
    testStringSerialize();
    testStringArrSerialize();
    testIntArrSerialize();
    testBoolArrSerialize();
    testCharStar();
    testSchemaSerialize();
    // // test serialize double
    // //serializer->serialize(1.3);
    // //serializer->deserializeDouble();

    // DoubleArray* darr = new DoubleArray();
    // darr->append(1.2);
    // darr->append(2.2);
    // //darr->serialize();
    // //DoubleArray* deserDA = darr->deserializeDoubleArray();

    

  LOG("Done.\n");
  return 0;
}