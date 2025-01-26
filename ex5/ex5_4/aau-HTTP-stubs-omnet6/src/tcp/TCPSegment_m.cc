//
// Generated file, do not edit! Created by opp_msgtool 6.1 from tcp/TCPSegment.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "TCPSegment_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(TCPSegment)

TCPSegment::TCPSegment(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

TCPSegment::TCPSegment(const TCPSegment& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

TCPSegment::~TCPSegment()
{
}

TCPSegment& TCPSegment::operator=(const TCPSegment& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void TCPSegment::copy(const TCPSegment& other)
{
    this->srcPort = other.srcPort;
    this->destPort = other.destPort;
    this->seqNr = other.seqNr;
    this->ackNr = other.ackNr;
    this->ack = other.ack;
    this->syn = other.syn;
    this->fin = other.fin;
    this->rst = other.rst;
    this->headerLength = other.headerLength;
    this->receiveWindow = other.receiveWindow;
    this->psh = other.psh;
    this->urg = other.urg;
}

void TCPSegment::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->srcPort);
    doParsimPacking(b,this->destPort);
    doParsimPacking(b,this->seqNr);
    doParsimPacking(b,this->ackNr);
    doParsimPacking(b,this->ack);
    doParsimPacking(b,this->syn);
    doParsimPacking(b,this->fin);
    doParsimPacking(b,this->rst);
    doParsimPacking(b,this->headerLength);
    doParsimPacking(b,this->receiveWindow);
    doParsimPacking(b,this->psh);
    doParsimPacking(b,this->urg);
}

void TCPSegment::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->srcPort);
    doParsimUnpacking(b,this->destPort);
    doParsimUnpacking(b,this->seqNr);
    doParsimUnpacking(b,this->ackNr);
    doParsimUnpacking(b,this->ack);
    doParsimUnpacking(b,this->syn);
    doParsimUnpacking(b,this->fin);
    doParsimUnpacking(b,this->rst);
    doParsimUnpacking(b,this->headerLength);
    doParsimUnpacking(b,this->receiveWindow);
    doParsimUnpacking(b,this->psh);
    doParsimUnpacking(b,this->urg);
}

int TCPSegment::getSrcPort() const
{
    return this->srcPort;
}

void TCPSegment::setSrcPort(int srcPort)
{
    this->srcPort = srcPort;
}

int TCPSegment::getDestPort() const
{
    return this->destPort;
}

void TCPSegment::setDestPort(int destPort)
{
    this->destPort = destPort;
}

int TCPSegment::getSeqNr() const
{
    return this->seqNr;
}

void TCPSegment::setSeqNr(int seqNr)
{
    this->seqNr = seqNr;
}

int TCPSegment::getAckNr() const
{
    return this->ackNr;
}

void TCPSegment::setAckNr(int ackNr)
{
    this->ackNr = ackNr;
}

bool TCPSegment::getAck() const
{
    return this->ack;
}

void TCPSegment::setAck(bool ack)
{
    this->ack = ack;
}

bool TCPSegment::getSyn() const
{
    return this->syn;
}

void TCPSegment::setSyn(bool syn)
{
    this->syn = syn;
}

bool TCPSegment::getFin() const
{
    return this->fin;
}

void TCPSegment::setFin(bool fin)
{
    this->fin = fin;
}

bool TCPSegment::getRst() const
{
    return this->rst;
}

void TCPSegment::setRst(bool rst)
{
    this->rst = rst;
}

int TCPSegment::getHeaderLength() const
{
    return this->headerLength;
}

void TCPSegment::setHeaderLength(int headerLength)
{
    this->headerLength = headerLength;
}

int TCPSegment::getReceiveWindow() const
{
    return this->receiveWindow;
}

void TCPSegment::setReceiveWindow(int receiveWindow)
{
    this->receiveWindow = receiveWindow;
}

bool TCPSegment::getPsh() const
{
    return this->psh;
}

void TCPSegment::setPsh(bool psh)
{
    this->psh = psh;
}

bool TCPSegment::getUrg() const
{
    return this->urg;
}

void TCPSegment::setUrg(bool urg)
{
    this->urg = urg;
}

class TCPSegmentDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_srcPort,
        FIELD_destPort,
        FIELD_seqNr,
        FIELD_ackNr,
        FIELD_ack,
        FIELD_syn,
        FIELD_fin,
        FIELD_rst,
        FIELD_headerLength,
        FIELD_receiveWindow,
        FIELD_psh,
        FIELD_urg,
    };
  public:
    TCPSegmentDescriptor();
    virtual ~TCPSegmentDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(TCPSegmentDescriptor)

TCPSegmentDescriptor::TCPSegmentDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(TCPSegment)), "omnetpp::cPacket")
{
    propertyNames = nullptr;
}

TCPSegmentDescriptor::~TCPSegmentDescriptor()
{
    delete[] propertyNames;
}

bool TCPSegmentDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<TCPSegment *>(obj)!=nullptr;
}

const char **TCPSegmentDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *TCPSegmentDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int TCPSegmentDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 12+base->getFieldCount() : 12;
}

unsigned int TCPSegmentDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_srcPort
        FD_ISEDITABLE,    // FIELD_destPort
        FD_ISEDITABLE,    // FIELD_seqNr
        FD_ISEDITABLE,    // FIELD_ackNr
        FD_ISEDITABLE,    // FIELD_ack
        FD_ISEDITABLE,    // FIELD_syn
        FD_ISEDITABLE,    // FIELD_fin
        FD_ISEDITABLE,    // FIELD_rst
        FD_ISEDITABLE,    // FIELD_headerLength
        FD_ISEDITABLE,    // FIELD_receiveWindow
        FD_ISEDITABLE,    // FIELD_psh
        FD_ISEDITABLE,    // FIELD_urg
    };
    return (field >= 0 && field < 12) ? fieldTypeFlags[field] : 0;
}

const char *TCPSegmentDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "srcPort",
        "destPort",
        "seqNr",
        "ackNr",
        "ack",
        "syn",
        "fin",
        "rst",
        "headerLength",
        "receiveWindow",
        "psh",
        "urg",
    };
    return (field >= 0 && field < 12) ? fieldNames[field] : nullptr;
}

int TCPSegmentDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "srcPort") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "destPort") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "seqNr") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "ackNr") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "ack") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "syn") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "fin") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "rst") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "headerLength") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "receiveWindow") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "psh") == 0) return baseIndex + 10;
    if (strcmp(fieldName, "urg") == 0) return baseIndex + 11;
    return base ? base->findField(fieldName) : -1;
}

const char *TCPSegmentDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_srcPort
        "int",    // FIELD_destPort
        "int",    // FIELD_seqNr
        "int",    // FIELD_ackNr
        "bool",    // FIELD_ack
        "bool",    // FIELD_syn
        "bool",    // FIELD_fin
        "bool",    // FIELD_rst
        "int",    // FIELD_headerLength
        "int",    // FIELD_receiveWindow
        "bool",    // FIELD_psh
        "bool",    // FIELD_urg
    };
    return (field >= 0 && field < 12) ? fieldTypeStrings[field] : nullptr;
}

const char **TCPSegmentDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *TCPSegmentDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int TCPSegmentDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    TCPSegment *pp = omnetpp::fromAnyPtr<TCPSegment>(object); (void)pp;
    switch (field) {
        default: return 0;
    }
}

void TCPSegmentDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    TCPSegment *pp = omnetpp::fromAnyPtr<TCPSegment>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'TCPSegment'", field);
    }
}

const char *TCPSegmentDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    TCPSegment *pp = omnetpp::fromAnyPtr<TCPSegment>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string TCPSegmentDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    TCPSegment *pp = omnetpp::fromAnyPtr<TCPSegment>(object); (void)pp;
    switch (field) {
        case FIELD_srcPort: return long2string(pp->getSrcPort());
        case FIELD_destPort: return long2string(pp->getDestPort());
        case FIELD_seqNr: return long2string(pp->getSeqNr());
        case FIELD_ackNr: return long2string(pp->getAckNr());
        case FIELD_ack: return bool2string(pp->getAck());
        case FIELD_syn: return bool2string(pp->getSyn());
        case FIELD_fin: return bool2string(pp->getFin());
        case FIELD_rst: return bool2string(pp->getRst());
        case FIELD_headerLength: return long2string(pp->getHeaderLength());
        case FIELD_receiveWindow: return long2string(pp->getReceiveWindow());
        case FIELD_psh: return bool2string(pp->getPsh());
        case FIELD_urg: return bool2string(pp->getUrg());
        default: return "";
    }
}

void TCPSegmentDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TCPSegment *pp = omnetpp::fromAnyPtr<TCPSegment>(object); (void)pp;
    switch (field) {
        case FIELD_srcPort: pp->setSrcPort(string2long(value)); break;
        case FIELD_destPort: pp->setDestPort(string2long(value)); break;
        case FIELD_seqNr: pp->setSeqNr(string2long(value)); break;
        case FIELD_ackNr: pp->setAckNr(string2long(value)); break;
        case FIELD_ack: pp->setAck(string2bool(value)); break;
        case FIELD_syn: pp->setSyn(string2bool(value)); break;
        case FIELD_fin: pp->setFin(string2bool(value)); break;
        case FIELD_rst: pp->setRst(string2bool(value)); break;
        case FIELD_headerLength: pp->setHeaderLength(string2long(value)); break;
        case FIELD_receiveWindow: pp->setReceiveWindow(string2long(value)); break;
        case FIELD_psh: pp->setPsh(string2bool(value)); break;
        case FIELD_urg: pp->setUrg(string2bool(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TCPSegment'", field);
    }
}

omnetpp::cValue TCPSegmentDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    TCPSegment *pp = omnetpp::fromAnyPtr<TCPSegment>(object); (void)pp;
    switch (field) {
        case FIELD_srcPort: return pp->getSrcPort();
        case FIELD_destPort: return pp->getDestPort();
        case FIELD_seqNr: return pp->getSeqNr();
        case FIELD_ackNr: return pp->getAckNr();
        case FIELD_ack: return pp->getAck();
        case FIELD_syn: return pp->getSyn();
        case FIELD_fin: return pp->getFin();
        case FIELD_rst: return pp->getRst();
        case FIELD_headerLength: return pp->getHeaderLength();
        case FIELD_receiveWindow: return pp->getReceiveWindow();
        case FIELD_psh: return pp->getPsh();
        case FIELD_urg: return pp->getUrg();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'TCPSegment' as cValue -- field index out of range?", field);
    }
}

void TCPSegmentDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    TCPSegment *pp = omnetpp::fromAnyPtr<TCPSegment>(object); (void)pp;
    switch (field) {
        case FIELD_srcPort: pp->setSrcPort(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_destPort: pp->setDestPort(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_seqNr: pp->setSeqNr(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_ackNr: pp->setAckNr(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_ack: pp->setAck(value.boolValue()); break;
        case FIELD_syn: pp->setSyn(value.boolValue()); break;
        case FIELD_fin: pp->setFin(value.boolValue()); break;
        case FIELD_rst: pp->setRst(value.boolValue()); break;
        case FIELD_headerLength: pp->setHeaderLength(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_receiveWindow: pp->setReceiveWindow(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_psh: pp->setPsh(value.boolValue()); break;
        case FIELD_urg: pp->setUrg(value.boolValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TCPSegment'", field);
    }
}

const char *TCPSegmentDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr TCPSegmentDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    TCPSegment *pp = omnetpp::fromAnyPtr<TCPSegment>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void TCPSegmentDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    TCPSegment *pp = omnetpp::fromAnyPtr<TCPSegment>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'TCPSegment'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

