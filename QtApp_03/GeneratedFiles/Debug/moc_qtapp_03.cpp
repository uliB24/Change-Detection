/****************************************************************************
** Meta object code from reading C++ file 'qtapp_03.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.0.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../qtapp_03.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtapp_03.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.0.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_QtApp_03_t {
    QByteArrayData data[14];
    char stringdata[220];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_QtApp_03_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_QtApp_03_t qt_meta_stringdata_QtApp_03 = {
    {
QT_MOC_LITERAL(0, 0, 8),
QT_MOC_LITERAL(1, 9, 4),
QT_MOC_LITERAL(2, 14, 0),
QT_MOC_LITERAL(3, 15, 10),
QT_MOC_LITERAL(4, 26, 18),
QT_MOC_LITERAL(5, 45, 19),
QT_MOC_LITERAL(6, 65, 19),
QT_MOC_LITERAL(7, 85, 20),
QT_MOC_LITERAL(8, 106, 22),
QT_MOC_LITERAL(9, 129, 20),
QT_MOC_LITERAL(10, 150, 5),
QT_MOC_LITERAL(11, 156, 20),
QT_MOC_LITERAL(12, 177, 20),
QT_MOC_LITERAL(13, 198, 20)
    },
    "QtApp_03\0open\0\0processing\0on_btnOpen_clicked\0"
    "on_btnClear_clicked\0on_btnPause_clicked\0"
    "on_btnResume_clicked\0on_btnSetParam_clicked\0"
    "txtChangeWinInput_01\0input\0"
    "txtChangeWinInput_02\0txtChangeWinInput_03\0"
    "txtChangeWinInput_04\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_QtApp_03[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x08,
       3,    0,   70,    2, 0x08,
       4,    0,   71,    2, 0x08,
       5,    0,   72,    2, 0x08,
       6,    0,   73,    2, 0x08,
       7,    0,   74,    2, 0x08,
       8,    0,   75,    2, 0x08,
       9,    1,   76,    2, 0x08,
      11,    1,   79,    2, 0x08,
      12,    1,   82,    2, 0x08,
      13,    1,   85,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   10,
    QMetaType::Void, QMetaType::QString,   10,

       0        // eod
};

void QtApp_03::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        QtApp_03 *_t = static_cast<QtApp_03 *>(_o);
        switch (_id) {
        case 0: _t->open(); break;
        case 1: _t->processing(); break;
        case 2: _t->on_btnOpen_clicked(); break;
        case 3: _t->on_btnClear_clicked(); break;
        case 4: _t->on_btnPause_clicked(); break;
        case 5: _t->on_btnResume_clicked(); break;
        case 6: _t->on_btnSetParam_clicked(); break;
        case 7: _t->txtChangeWinInput_01((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->txtChangeWinInput_02((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 9: _t->txtChangeWinInput_03((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 10: _t->txtChangeWinInput_04((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject QtApp_03::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QtApp_03.data,
      qt_meta_data_QtApp_03,  qt_static_metacall, 0, 0}
};


const QMetaObject *QtApp_03::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QtApp_03::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QtApp_03.stringdata))
        return static_cast<void*>(const_cast< QtApp_03*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QtApp_03::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
