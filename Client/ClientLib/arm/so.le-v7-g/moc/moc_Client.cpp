/****************************************************************************
** Meta object code from reading C++ file 'Client.hpp'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/Client.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Client.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Client[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x05,
      23,    7,    7,    7, 0x05,
      37,    7,    7,    7, 0x05,

 // slots: signature, parameters, type, tag, flags
      93,   66,   57,    7, 0x0a,
     148,   66,   57,    7, 0x0a,
     228,    7,  208,    7, 0x0a,
     247,    7,  239,    7, 0x0a,
     264,    7,  239,    7, 0x0a,
     284,  280,    7,    7, 0x0a,
     318,  310,    7,    7, 0x0a,
     346,    7,    7,    7, 0x0a,

 // methods: signature, parameters, type, tag, flags
     363,  280,    7,    7, 0x02,
     398,  386,    7,    7, 0x02,
     426,  310,    7,    7, 0x02,
     457,    7,    7,    7, 0x02,
     489,  465,    7,    7, 0x02,
     530,    7,    7,    7, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_Client[] = {
    "Client\0\0slideChanged()\0messageSent()\0"
    "loginStateChanged()\0Message*\0"
    "parameters,parameter_types\0"
    "setSlide(QMap<QString,QVariant>,QMap<QString,QString>)\0"
    "loginResponse(QMap<QString,QVariant>,QMap<QString,QString>)\0"
    "bb::cascades::Image\0getSlide()\0QString\0"
    "getLastSentMsg()\0getLoginState()\0msg\0"
    "onMessageParsed(Message*)\0message\0"
    "simulateSocketSent(QString)\0"
    "connectionLost()\0invokeRemote(Message*)\0"
    "msg,cleanup\0invokeRemote(Message*,bool)\0"
    "simulateSocketReceive(QString)\0login()\0"
    "addr,cmd_port,data_port\0"
    "connectToServer(QString,QString,QString)\0"
    "requestImage()\0"
};

void Client::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Client *_t = static_cast<Client *>(_o);
        switch (_id) {
        case 0: _t->slideChanged(); break;
        case 1: _t->messageSent(); break;
        case 2: _t->loginStateChanged(); break;
        case 3: { Message* _r = _t->setSlide((*reinterpret_cast< QMap<QString,QVariant>(*)>(_a[1])),(*reinterpret_cast< QMap<QString,QString>(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Message**>(_a[0]) = _r; }  break;
        case 4: { Message* _r = _t->loginResponse((*reinterpret_cast< QMap<QString,QVariant>(*)>(_a[1])),(*reinterpret_cast< QMap<QString,QString>(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< Message**>(_a[0]) = _r; }  break;
        case 5: { bb::cascades::Image _r = _t->getSlide();
            if (_a[0]) *reinterpret_cast< bb::cascades::Image*>(_a[0]) = _r; }  break;
        case 6: { QString _r = _t->getLastSentMsg();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 7: { QString _r = _t->getLoginState();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 8: _t->onMessageParsed((*reinterpret_cast< Message*(*)>(_a[1]))); break;
        case 9: _t->simulateSocketSent((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->connectionLost(); break;
        case 11: _t->invokeRemote((*reinterpret_cast< Message*(*)>(_a[1]))); break;
        case 12: _t->invokeRemote((*reinterpret_cast< Message*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 13: _t->simulateSocketReceive((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 14: _t->login(); break;
        case 15: _t->connectToServer((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 16: _t->requestImage(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Client::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Client::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Client,
      qt_meta_data_Client, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Client::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Client::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Client::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Client))
        return static_cast<void*>(const_cast< Client*>(this));
    return QObject::qt_metacast(_clname);
}

int Client::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 17)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 17;
    }
    return _id;
}

// SIGNAL 0
void Client::slideChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Client::messageSent()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Client::loginStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
