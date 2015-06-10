/****************************************************************************
** Meta object code from reading C++ file 'ServerSocket.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/ServerSocket.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ServerSocket.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ServerSocket[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   14,   13,   13, 0x05,
      44,   35,   13,   13, 0x05,
      67,   13,   13,   13, 0x05,
      97,   83,   13,   13, 0x05,
     136,   83,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     176,   13,   13,   13, 0x08,
     198,   35,   13,   13, 0x08,
     256,  227,   13,   13, 0x08,
     321,  296,  291,   13, 0x0a,
     353,   13,   13,   13, 0x0a,
     387,  367,   13,   13, 0x0a,
     417,  227,  413,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ServerSocket[] = {
    "ServerSocket\0\0newIP\0newIP(QString)\0"
    "clientID\0clientDisconnect(uint)\0"
    "stoppedServer()\0data,clientID\0"
    "receivedCmdFromClient(QByteArray,uint)\0"
    "receivedDataFromClient(QByteArray,uint)\0"
    "handleNewConnection()\0"
    "handleClientDisconnect(uint)\0"
    "data,clientID,connectionType\0"
    "handleNewRead(QByteArray,uint,int)\0"
    "bool\0cmdPort_str,dataPort_str\0"
    "beginListening(QString,QString)\0"
    "closeServer()\0data,connectionType\0"
    "sendToAll(QByteArray,int)\0int\0"
    "sendToID(QByteArray,uint,int)\0"
};

void ServerSocket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ServerSocket *_t = static_cast<ServerSocket *>(_o);
        switch (_id) {
        case 0: _t->newIP((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->clientDisconnect((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 2: _t->stoppedServer(); break;
        case 3: _t->receivedCmdFromClient((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 4: _t->receivedDataFromClient((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 5: _t->handleNewConnection(); break;
        case 6: _t->handleClientDisconnect((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 7: _t->handleNewRead((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 8: { bool _r = _t->beginListening((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 9: _t->closeServer(); break;
        case 10: _t->sendToAll((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: { int _r = _t->sendToID((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ServerSocket::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ServerSocket::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ServerSocket,
      qt_meta_data_ServerSocket, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ServerSocket::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ServerSocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ServerSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ServerSocket))
        return static_cast<void*>(const_cast< ServerSocket*>(this));
    return QObject::qt_metacast(_clname);
}

int ServerSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void ServerSocket::newIP(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ServerSocket::clientDisconnect(uint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ServerSocket::stoppedServer()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void ServerSocket::receivedCmdFromClient(QByteArray _t1, uint _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void ServerSocket::receivedDataFromClient(QByteArray _t1, uint _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
