/****************************************************************************
** Meta object code from reading C++ file 'CompoundShapeWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/scene/controller/objects/bodyshape/CompoundShapeWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CompoundShapeWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_urchin__CompoundShapeWidget_t {
    QByteArrayData data[5];
    char stringdata[109];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_urchin__CompoundShapeWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_urchin__CompoundShapeWidget_t qt_meta_stringdata_urchin__CompoundShapeWidget = {
    {
QT_MOC_LITERAL(0, 0, 27),
QT_MOC_LITERAL(1, 28, 20),
QT_MOC_LITERAL(2, 49, 0),
QT_MOC_LITERAL(3, 50, 28),
QT_MOC_LITERAL(4, 79, 28)
    },
    "urchin::CompoundShapeWidget\0"
    "addNewLocalizedShape\0\0"
    "removeSelectedLocalizedShape\0"
    "updateSelectedLocalizedShape\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_urchin__CompoundShapeWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   29,    2, 0x08,
       3,    0,   30,    2, 0x08,
       4,    0,   31,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void urchin::CompoundShapeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CompoundShapeWidget *_t = static_cast<CompoundShapeWidget *>(_o);
        switch (_id) {
        case 0: _t->addNewLocalizedShape(); break;
        case 1: _t->removeSelectedLocalizedShape(); break;
        case 2: _t->updateSelectedLocalizedShape(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject urchin::CompoundShapeWidget::staticMetaObject = {
    { &BodyShapeWidget::staticMetaObject, qt_meta_stringdata_urchin__CompoundShapeWidget.data,
      qt_meta_data_urchin__CompoundShapeWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *urchin::CompoundShapeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *urchin::CompoundShapeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_urchin__CompoundShapeWidget.stringdata))
        return static_cast<void*>(const_cast< CompoundShapeWidget*>(this));
    if (!strcmp(_clname, "Observer"))
        return static_cast< Observer*>(const_cast< CompoundShapeWidget*>(this));
    return BodyShapeWidget::qt_metacast(_clname);
}

int urchin::CompoundShapeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = BodyShapeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
