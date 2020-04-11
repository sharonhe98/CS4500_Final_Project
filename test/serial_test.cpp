#pragma once
#include <cstdlib>
#include <unistd.h>
#include "string.h"
#include <string.h>
#include <assert.h>
#include "../src/helper.h"
#include "../src/kvstore/kvstore.h"
#include "../src/dataframe/sorer.h"

#define LOG(...) fprintf(stderr, "(" __FILE__ ") " __VA_ARGS__);

void testStringSerialize()
{
    Serializer serializer;
    String *strfoo = new String("foo");
    serializer.write(strfoo);

    char *result = serializer.getSerChar();
    Deserializer *deserializer = new Deserializer(result);
    String *s = deserializer->readString();
    assert(strfoo->equals(s));
    delete strfoo;
    delete deserializer;
    printf("String serialize passed!\n");
}

void testStringArrSerialize()
{
    Serializer *serializer = new Serializer();
    StringArray *sa = new StringArray();
    String *strfoo = new String("foo");
    String *strbar = new String("bar");

    assert(sa->length() == 0);
    sa->append(strfoo);
    assert(sa->length() == 1);
    sa->append(strbar);
    assert(sa->length() == 2);
    char *result = sa->serialize(serializer);
    Deserializer *deserializer = new Deserializer(result);
    StringArray *dsa = sa->deserializeStringArray(deserializer);
    assert(dsa->length() == 2);
    String *s1 = dsa->get(0);
    assert(s1->equals(sa->get(0)));
    String *s2 = dsa->get(1);
    assert(s2->equals(sa->get(1)));
    
    delete serializer;
    //delete [] sa;
    delete strfoo;
    delete strbar;
    delete deserializer;
    printf("test StringArray de/serialize passed!\n");
}

void testIntArrSerialize()
{
    Serializer *serializer = new Serializer();
    IntArray *ia = new IntArray();

    assert(ia->length() == 0);
    ia->append(1);
    assert(ia->length() == 1);
    ia->append(2);
    assert(ia->length() == 2);
    ia->append(3);
    assert(ia->length() == 3);
    char *result = ia->serialize(serializer);
    Deserializer *deserializer = new Deserializer(result);
    IntArray *dia = ia->deserializeIntArray(deserializer);
    assert(dia->length() == 3);
    int i1 = dia->get(0);
    assert(i1 == ia->get(0));
    int i2 = dia->get(1);
    assert(i2 == ia->get(1));
    int i3 = dia->get(2);
    assert(i3 == ia->get(2));
    delete serializer;
    // delete[] ia;
    delete deserializer;
    printf("test IntArray de/serialize passed!\n");
}

void testBoolArrSerialize()
{
    Serializer *serializer = new Serializer();
    BoolArray *ba = new BoolArray();

    assert(ba->length() == 0);
    ba->append(1);
    assert(ba->length() == 1);
    ba->append(0);
    assert(ba->length() == 2);
    ba->append(1);
    assert(ba->length() == 3);
    char *result = ba->serialize(serializer);
    Deserializer *deserializer = new Deserializer(result);
    BoolArray *dba = ba->deserializeBoolArray(deserializer);
    assert(dba->length() == 3);
    bool b1 = dba->get(0);
    assert(b1 == ba->get(0));
    bool b2 = dba->get(1);
    assert(b2 == ba->get(1));
    bool b3 = dba->get(2);
    assert(b3 == ba->get(2));
    delete serializer;
    // delete ba;
    delete deserializer;
    printf("test BoolArray de/serialize passed!\n");
}

void testCharStar()
{
    Serializer *serializer = new Serializer();
    char *types = "IFSB";
    serializer->write(types);
    char *result = serializer->getSerChar();
    Deserializer *deserializer = new Deserializer(result);
    char *deChar = deserializer->readChars();
    assert(types == deChar);
    assert(strlen(types) == strlen(deChar));
    assert(types[1] == deChar[1]);
    delete serializer;
    delete deserializer;
    printf("test char* passed\n");
}

void testChar()
{
    Serializer *serializer = new Serializer();
    char type = 'S';
    serializer->write(type);
    char *result = serializer->getSerChar();
    Deserializer *deserializer = new Deserializer(result);
    char deChar = deserializer->readChar();
    assert(type == deChar);
    assert(deChar == 'S');
    delete serializer;
    delete deserializer;
    printf("test char passed\n");
}

void testSchemaSerialize()
{
    Serializer *serializer = new Serializer();
    Schema *isfb = new Schema("ISFB");
    char* result = isfb->serialize(serializer);
    Deserializer *deserializer = new Deserializer(result);
    Schema *deSchema = isfb->deserialize(deserializer);
    assert(isfb->track_types == deSchema->track_types);
    delete serializer;
    delete deserializer;
    printf("Schema serialize success!\n");
}

// void testColumnSerialize()
// {
//     Serializer *serializer = new Serializer();
//     StringColumn* sc = new StringColumn();
// 	String* hello = new String("hello");
// 	for (size_t i = 0; i < 1000 * 2; i++) {
// 		sc->push_back(hello);
// 	}
// 	assert(sc->currentChunk_ == 1);
//     assert(sc->get(0)->c_str() == hello->c_str());
//     assert(sc->get(1999)->c_str() == hello->c_str());
//     printf("current chunk is:%zu\n", sc->currentChunk_);
//     sc->serialize(serializer);
//     char* result = serializer->getSerChar();
//     printf("serialized!\n");
//     printf("pos is at: %zu\n", serializer->getPos());
//     Deserializer *deserializer = new Deserializer(result);
//     StringColumn *c = StringColumn::deserialize(deserializer);
//     printf("type %c sctype %c\n", c->type_, sc->type_);
//     assert(c->type_ == sc->type_);
//     printf("chunk %zu\n", c->currentChunk_);
//     assert(c->currentChunk_ == 1);
//     printf("String is %s Hello is %s\n", c->get(0)->c_str(), hello->c_str());
//     assert(c->get(0)->c_str() == hello->c_str());
//     assert(c->get(1999));
//     delete serializer;
//     delete deserializer;
//     printf("Column serialize success!\n");
// }

void testColumnSerialize()
{
    Serializer *serializer = new Serializer();
    StringColumn* sc = new StringColumn();
	String* hello = new String("hello");
	for (size_t i = 0; i < 1000 * 2; i++) {
		sc->push_back(hello);
	}
    assert(sc->get(0)->c_str() == hello->c_str());
    assert(sc->get(1999)->c_str() == hello->c_str());
    sc->serialize(serializer);
    char* result = serializer->getSerChar();
    printf("serialized!\n");
    Deserializer *deserializer = new Deserializer(result);
    Column *c = Column::deserialize(deserializer);
    // printf("type %c sctype %c\n", c->type_, sc->type_);
    assert(c->type_ == sc->type_);
    // printf("chunk %zu\n", c->currentChunk_);
    // Sys sys;
    StringColumn *sc2 = dynamic_cast<StringColumn*>(c);
    assert(strcmp(sc2->get(0)->c_str(), hello->c_str()) == 0);
    assert(strcmp(sc2->get(1999)->c_str(), hello->c_str()) == 0);
    delete serializer;
    delete deserializer;
    printf("Column serialize success!\n");
}

void testDFSerialize()
{
	FILE *f = fopen("../src/data.sor", "r");
	SOR *sor = new SOR();
	char *schemaFromFile = sor->getSchema(f, 0, 1000000);
	Schema s(schemaFromFile);

	DataFrame *df = sor->setDataFrame(f, 0, 100000);
	df->print();
	printf("Build DF from file passed!\n");
    Serializer *ser = new Serializer();
    df->serialize(ser);
    char* result = ser->getSerChar();
    Deserializer* dser = new Deserializer(result);
    DataFrame* deDF = new DataFrame(dser);
    assert(df->scm.track_types == deDF->scm.track_types);
    deDF->print();
    printf("Serialize DF from file passed!\n");

}

void testMessageSerialize()
{
    String *good = new String("good");
    Status *status = new Status(MsgKind::Status, 1, 2, 3, good);
    Serializer *ser = new Serializer();
    status->serialize(ser);
    char *result = ser->getSerChar();
    assert(result);
    Deserializer *dser = new Deserializer(result);
    Message *msg = Message::deserializeMsg(dser);
    assert(msg->kind_ == status->kind_);
    assert((int)msg->kind_ == 6);
    assert(msg->sender_ == status->sender_);
    assert(msg->target_ == status->target_);
    assert(msg->id_ == status->id_);
    printf("deserialize msg successfully\n");
    delete good;
    delete status;
    delete ser;
}

int main(int argc, char **argv)
{
    testStringSerialize();
    testStringArrSerialize();
    testIntArrSerialize();
    testBoolArrSerialize();
    testCharStar();
    testChar();
    testSchemaSerialize();
    testColumnSerialize();
    //testDFSerialize();
    testMessageSerialize();

    LOG("Done.\n");
    return 0;
}