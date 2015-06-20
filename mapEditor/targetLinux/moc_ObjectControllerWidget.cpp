/****************************************************************************
** Meta object code from reading C++ file 'ObjectControllerWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/scene/controller/objects/ObjectControllerWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ObjectControllerWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_urchin__ObjectControllerWidget_t {
    QByteArrayData data[12];
    char stringdata[262];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_urchin__ObjectControllerWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_urchin__ObjectControllerWidget_t qt_meta_stringdata_urchin__ObjectControllerWidget = {
    {
QT_MOC_LITERAL(0, 0, 30),
QT_MOC_LITERAL(1, 31, 19),
QT_MOC_LITERAL(2, 51, 0),
QT_MOC_LITERAL(3, 52, 20),
QT_MOC_LITERAL(4, 73, 21),
QT_MOC_LITERAL(5, 95, 17),
QT_MOC_LITERAL(6, 113, 17),
QT_MOC_LITERAL(7, 131, 25),
QT_MOC_LITERAL(8, 157, 16),
QT_MOC_LITERAL(9, 174, 29),
QT_MOC_LITERAL(10, 204, 16),
QT_MOC_LITERAL(11, 221, 39)
    },
    "urchin::ObjectControllerWidget\0"
    "showAddObjectDialog\0\0removeSelectedObject\0"
    "updateObjectTransform\0updateObjectScale\0"
    "updateObjectFlags\0showChangeBodyShapeDialog\0"
    "rigidBodyToggled\0updateObjectPhysicsProperties\0"
    "bodyShapeChanged\0"
    "std::shared_ptr<const CollisionShape3D>\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_urchin__ObjectControllerWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x08,
       3,    0,   60,    2, 0x08,
       4,    0,   61,    2, 0x08,
       5,    0,   62,    2, 0x08,
       6,    0,   63,    2, 0x08,
       7,    0,   64,    2, 0x08,
       8,    1,   65,    2, 0x08,
       9,    0,   68,    2, 0x08,
      10,    1,   69,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,    2,

       0        // eod
};

void urchin::ObjectControllerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ObjectControllerWidget *_t = static_cast<ObjectControllerWidget *>(_o);
        switch (_id) {
        case 0: _t->showAddObjectDialog(); break;
        case 1: _t->removeSelectedObject(); break;
        case 2: _t->updateObjectTransform(); break;
        case 3: _t->updateObjectScale(); break;
        case 4: _t->updateObjectFlags(); break;
        case 5: _t->showChangeBodyShapeDialog(); break;
        case 6: _t->rigidBodyToggled((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->updateObjectPhysicsProperties(); break;
        case 8: _t->bodyShapeChanged((*reinterpret_cast< std::shared_ptr<const CollisionShape3D>(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject urchin::ObjectControllerWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_urchin__ObjectControllerWidget.data,
      qt_meta_data_urchin__ObjectControllerWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *urchin::ObjectControllerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *urchin::ObjectControllerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_urchin__ObjectControllerWidget.stringdata))
        return static_cast<void*>(const_cast< ObjectControllerWidget*>(this));
    if (!strcmp(_clname, "Observer"))
        return static_cast< Observer*>(const_cast< ObjectControllerWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int urchin::ObjectControllerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
