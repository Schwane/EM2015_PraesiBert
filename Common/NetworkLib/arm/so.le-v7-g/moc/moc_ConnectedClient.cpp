/****************************************************************************
** Meta object code from reading C++ file 'ConnectedClient.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/ConnectedClient.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ConnectedClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ConnectedClient[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      37,   28,   16,   16, 0x05,
      85,   56,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
     114,   16,   16,   16, 0x0a,
     124,   16,   16,   16, 0x0a,
     140,   16,   16,   16, 0x0a,
     157,   16,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ConnectedClient[] = {
    "ConnectedClient\0\0finished()\0clientID\0"
    "disconnected(uint)\0data,clientID,connectionType\0"
    "newData(QByteArray,uint,int)\0process()\0"
    "handleCmdRead()\0handleDataRead()\0"
    "handleDisconnect()\0"
};

void ConnectedClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ConnectedClient *_t = static_cast<ConnectedClient *>(_o);
        switch (_id) {
        case 0: _t->finished(); break;
        case 1: _t->disconnected((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 2: _t->newData((*reinterpret_cast< QByteArray(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->process(); break;
        case 4: _t->handleCmdRead(); break;
        case 5: _t->handleDataRead(); break;
        case 6: _t->handleDisconnect(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ConnectedClient::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ConnectedClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ConnectedClient,
      qt_meta_data_ConnectedClient, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ConnectedClient::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ConnectedClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ConnectedClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ConnectedClient))
        return static_cast<void*>(const_cast< ConnectedClient*>(this));
    return QObject::qt_metacast(_clname);
}

int ConnectedClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ConnectedClient::finished()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ConnectedClient::disconnected(uint _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ConnectedClient::newData(QByteArray _t1, uint _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
