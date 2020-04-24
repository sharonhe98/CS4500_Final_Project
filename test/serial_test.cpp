#include <cstdlib>
#include <unistd.h>
#include "string.h"
#include <string.h>
#include <assert.h>
#include "../src/helper.h"
#include "../src/kvstore/kvstore.h"

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
    StringArray *dsa = StringArray::deserializeStringArray(deserializer);
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
    IntArray *dia = IntArray::deserializeIntArray(deserializer);
    assert(dia->length() == 3);
    int i1 = dia->get(0);
    assert(i1 == ia->get(0));
    int i2 = dia->get(1);
    assert(i2 == ia->get(1));
    int i3 = dia->get(2);
    assert(i3 == ia->get(2));
    delete serializer;
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
    BoolArray *dba = BoolArray::deserializeBoolArray(deserializer);
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
    printf("dechar length is %zu\n", strlen(deChar));
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
    isfb->serialize(serializer);
    char* result = serializer->getSerChar();
    Deserializer *deserializer = new Deserializer(result);
    Schema *deSchema = Schema::deserialize(deserializer);
    assert(isfb->track_types == deSchema->track_types);
    delete serializer;
    delete deserializer;
    printf("Schema serialize success!\n");
}

void testStrColumnSerialize()
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
    assert(c->type_ == sc->type_);
    StringColumn *sc2 = dynamic_cast<StringColumn*>(c);
    assert(strcmp(sc2->get(0)->c_str(), hello->c_str()) == 0);
    assert(strcmp(sc2->get(1999)->c_str(), hello->c_str()) == 0);
    assert(sc2->get_type() == 'S');
    assert(sc2->chunks_.length() == 2);
    delete serializer;
    delete deserializer;
    printf("String Column serialize success!\n");
}

void testIntColumnSerialize()
{
    Serializer *serializer = new Serializer();
    IntColumn* ic = new IntColumn();
	for (size_t i = 0; i < 1000 * 2; i++) {
		ic->push_back(2);
	}
    assert(ic->get(0) == 2);
    assert(ic->get(1999) == 2);
    ic->serialize(serializer);
    char* result = serializer->getSerChar();
    printf("serialized!\n");
    Deserializer *deserializer = new Deserializer(result);
    Column *c = Column::deserialize(deserializer);
    assert(c->type_ == ic->type_);

    IntColumn *ic2 = dynamic_cast<IntColumn*>(c);
    assert(ic2->get(0) == 2);
    assert(ic2->get(1999) == 2);
    assert(ic2->get_type() == 'I');
    assert(ic2->chunks_.length() == 2);
    delete serializer;
    delete deserializer;
    printf("Int Column serialize success!\n");
}

void testDFSerialize()
{
	FILE *f = fopen("test/data.sor", "r");
    assert(f);
	SOR *sor = new SOR();
	char *schemaFromFile = sor->getSchema(f, 0, 1000000);
	Schema s(schemaFromFile);
	DataFrame *df = sor->setDataFrame(f, 0, 100000);
    assert(df->cols[0]->get_type() == 'B');
    assert(df->cols[1]->get_type() == 'I');
    assert(df->cols[2]->get_type() == 'S');
    assert(df->cols[3]->get_type() == 'F');
    assert(df->cols[4]->get_type() == 'I');
    assert(df->ncols() == 5);
    assert(df->nrows() == 5);
	df->print();
	printf("Build DF from file passed!\n");
    Serializer *ser = new Serializer();
    df->serialize(ser);
    char* result = ser->getSerChar();
    Deserializer* dser = new Deserializer(result);
    DataFrame* deDF = DataFrame::deserialize(dser);
    assert(deDF->cols[0]->get_type() == 'B');
    assert(deDF->cols[1]->get_type() == 'I');
    assert(deDF->cols[2]->get_type() == 'S');
    assert(deDF->cols[3]->get_type() == 'F');
    assert(deDF->cols[4]->get_type() == 'I');
    assert(deDF->ncols() == 5);
    assert(deDF->nrows() == 5);
    deDF->print();
    printf("Serialize DF from file passed!\n");

}

void testStatusMessageSerialize()
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
    printf("deserialize Status msg successfully\n");
    delete good;
    delete status;
    delete ser;
}

void testRegisterMessageSerialize()
{
    size_t port = 8080;
    sockaddr_in clientAddress;
    clientAddress.sin_family = AF_INET; 
    clientAddress.sin_addr.s_addr = INADDR_ANY; 
    clientAddress.sin_port = htons( port );
    Register *reg = new Register(MsgKind::Register, 0, 1, 0, clientAddress, port);
    Serializer *ser = new Serializer();
    reg->serialize(ser);
    char *result = ser->getSerChar();
    assert(result);
    Deserializer *dser = new Deserializer(result);
    Message *msg = Message::deserializeMsg(dser);
    assert(msg->kind_ == reg->kind_);
    assert((int)msg->kind_ == 8);
    assert(msg->sender_ == reg->sender_);
    assert(msg->target_ == reg->target_);
    assert(msg->id_ == reg->id_);
    Register *reg2 = dynamic_cast<Register*>(msg);
    assert(reg->port_ == reg2->port_);
    assert(reg2->client_.sin_port);
    printf("deserialize Register msg successfully\n");
    delete ser;
}

void testDirectoryMessageSerialize()
{
    size_t client = 2;
    IntArray * ports = new IntArray();
    ports->append(1);
    ports->append(2);
    assert(ports->length() == 2);
    StringArray* addresses = new StringArray();
    String * addr = new String("127.0.0.0");
    addresses->append(addr);
    addresses->append(addr);
    assert(addresses->length() == 2);
    Directory *dir = new Directory(MsgKind::Directory, 0, 1, 0, client, ports, addresses);
    Serializer *ser = new Serializer();
    dir->serialize(ser);
    char *result = ser->getSerChar();
    assert(result);
    Deserializer *dser = new Deserializer(result);
    Message *msg = Message::deserializeMsg(dser);
    assert(msg->kind_ == dir->kind_);
    assert((int)msg->kind_ == 9);
    assert(msg->sender_ == dir->sender_);
    assert(msg->target_ == dir->target_);
    assert(msg->id_ == dir->id_);
    Directory *dir2 = dynamic_cast<Directory*>(msg);
    assert(dir2->client_ == client);
    assert(dir2->ports_->length() == 2);
    assert(dir2->addresses_->length() == 2);
    printf("deserialize Directory msg successfully\n");
    delete ser;
}

void testKey() {
    Key *verify = new Key("verif", 0);
    Serializer *s1 = new Serializer();
    verify->serialize(s1);
    char* result = s1->getSerChar();
    Deserializer *dser = new Deserializer(result);
    Key * deKey = Key::deserialize(dser);
    assert(strcmp(deKey->key->c_str(), verify->key->c_str()) == 0);
    printf("Key deserialized!\n");

}

void testValue() {
    Key *verify = new Key("verif", 0);
    Serializer *s1 = new Serializer();
    verify->serialize(s1);
    char* result = s1->getSerChar();
    Value * val = new Value(result);
    Serializer * valser = new Serializer();
    val->serialize(valser);
    char* buf = valser->getSerChar();
    Deserializer *dser = new Deserializer(buf);
    Value * v = Value::deserialize(dser);
    Deserializer * dedata = new Deserializer(v->data_);
    Key * deKey = Key::deserialize(dedata);
    assert(strcmp(deKey->key->c_str(), verify->key->c_str()) == 0);
    printf("Key in value deserialized!\n");

}

void testKeyInData() {
    Key *verify = new Key("verif", 0);
    Serializer *s1 = new Serializer();
    verify->serialize(s1);
    printf("key buff length is : %zu\n", s1->getPos());
	Value *v1 = new Value(s1->getSerChar());
	Data *da = new Data(MsgKind::Data, 1, 0, 2, v1);
    Serializer *ss = new Serializer();
    da->serialize(ss);
    char* buffer = ss->getSerChar();
    printf("buffer size is: %zu\n", ss->getPos());
    Deserializer* des = new Deserializer(buffer);
	Message *msg = Message::deserializeMsg(des);
    assert((int)msg->getKind() == (int)MsgKind::Data);
    Data *d = dynamic_cast<Data*>(msg);
	Value *v = d->v_;
    printf("deserizliezd msg!\n");
    assert(v->data_);
    printf("data!\n");
	Deserializer *dee = new Deserializer(v->data_);
    printf("gets here?\n");
	Key* wag = Key::deserialize(dee);
    assert(strcmp(wag->key->c_str(), verify->key->c_str()) == 0);
    printf("Deserialized Data!\n");
}

void testData() {
    Key *verify = new Key("verif", 0);
    Serializer *s1 = new Serializer();
	verify->serialize(s1);
	Value *v1 = new Value(s1->getSerChar());
	Data *da = new Data(MsgKind::Data, 1, 0, 2, v1);
    Serializer* ser = new Serializer();
	da->serialize(ser);
	char *buffer = ser->getSerChar();
	size_t size = ser->getPos();
	if (da->getKind() == MsgKind::Data) {
	Deserializer * d = new Deserializer(buffer);
	Message * m = Message::deserializeMsg(d);
	assert(m->getKind() == MsgKind::Data);
	Data * dat = dynamic_cast<Data*>(m);
	Value * v = dat->v_;
	Deserializer * vdser = new Deserializer(v->data_);
	Key * expectedKey = Key::deserialize(vdser);
	printf("expected key name kvwaiteget%s\n", expectedKey->key->c_str());

}
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
    testStrColumnSerialize();
    testIntColumnSerialize();
    testStatusMessageSerialize();
    testRegisterMessageSerialize();
    testDirectoryMessageSerialize();
    testDFSerialize();
    testKey();
    testValue();
    testKeyInData();
    testData();

    LOG("Done.\n");
    return 0;
}