/****************************************************************************
** Meta object code from reading C++ file 'LightControllerWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/scene/controller/lights/LightControllerWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LightControllerWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_urchin__LightControllerWidget_t {
    QByteArrayData data[6];
    char stringdata[130];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_urchin__LightControllerWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_urchin__LightControllerWidget_t qt_meta_stringdata_urchin__LightControllerWidget = {
    {
QT_MOC_LITERAL(0, 0, 29),
QT_MOC_LITERAL(1, 30, 18),
QT_MOC_LITERAL(2, 49, 0),
QT_MOC_LITERAL(3, 50, 19),
QT_MOC_LITERAL(4, 70, 28),
QT_MOC_LITERAL(5, 99, 29)
    },
    "urchin::LightControllerWidget\0"
    "showAddLightDialog\0\0removeSelectedLight\0"
    "updateLightGeneralProperties\0"
    "updateLightSpecificProperties\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_urchin__LightControllerWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08,
       3,    0,   35,    2, 0x08,
       4,    0,   36,    2, 0x08,
       5,    0,   37,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void urchin::LightControllerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LightControllerWidget *_t = static_cast<LightControllerWidget *>(_o);
        switch (_id) {
        case 0: _t->showAddLightDialog(); break;
        case 1: _t->removeSelectedLight(); break;
        case 2: _t->updateLightGeneralProperties(); break;
        case 3: _t->updateLightSpecificProperties(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject urchin::LightControllerWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_urchin__LightControllerWidget.data,
      qt_meta_data_urchin__LightControllerWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *urchin::LightControllerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *urchin::LightControllerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_urchin__LightControllerWidget.stringdata))
        return static_cast<void*>(const_cast< LightControllerWidget*>(this));
    if (!strcmp(_clname, "Observer"))
        return static_cast< Observer*>(const_cast< LightControllerWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int urchin::LightControllerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
