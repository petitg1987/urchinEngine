/****************************************************************************
** Meta object code from reading C++ file 'SoundControllerWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/scene/controller/sounds/SoundControllerWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SoundControllerWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_urchin__SoundControllerWidget_t {
    QByteArrayData data[11];
    char stringdata[252];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_urchin__SoundControllerWidget_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_urchin__SoundControllerWidget_t qt_meta_stringdata_urchin__SoundControllerWidget = {
    {
QT_MOC_LITERAL(0, 0, 29),
QT_MOC_LITERAL(1, 30, 18),
QT_MOC_LITERAL(2, 49, 0),
QT_MOC_LITERAL(3, 50, 19),
QT_MOC_LITERAL(4, 70, 28),
QT_MOC_LITERAL(5, 99, 29),
QT_MOC_LITERAL(6, 129, 29),
QT_MOC_LITERAL(7, 159, 28),
QT_MOC_LITERAL(8, 188, 26),
QT_MOC_LITERAL(9, 215, 17),
QT_MOC_LITERAL(10, 233, 17)
    },
    "urchin::SoundControllerWidget\0"
    "showAddSoundDialog\0\0removeSelectedSound\0"
    "updateSoundGeneralProperties\0"
    "updateSoundSpecificProperties\0"
    "updateSoundBehaviorProperties\0"
    "showChangeSoundTriggerDialog\0"
    "showChangeSoundShapeDialog\0soundShapeChanged\0"
    "const SoundShape*\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_urchin__SoundControllerWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08,
       3,    0,   55,    2, 0x08,
       4,    0,   56,    2, 0x08,
       5,    0,   57,    2, 0x08,
       6,    0,   58,    2, 0x08,
       7,    0,   59,    2, 0x08,
       8,    0,   60,    2, 0x08,
       9,    1,   61,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,    2,

       0        // eod
};

void urchin::SoundControllerWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SoundControllerWidget *_t = static_cast<SoundControllerWidget *>(_o);
        switch (_id) {
        case 0: _t->showAddSoundDialog(); break;
        case 1: _t->removeSelectedSound(); break;
        case 2: _t->updateSoundGeneralProperties(); break;
        case 3: _t->updateSoundSpecificProperties(); break;
        case 4: _t->updateSoundBehaviorProperties(); break;
        case 5: _t->showChangeSoundTriggerDialog(); break;
        case 6: _t->showChangeSoundShapeDialog(); break;
        case 7: _t->soundShapeChanged((*reinterpret_cast< const SoundShape*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject urchin::SoundControllerWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_urchin__SoundControllerWidget.data,
      qt_meta_data_urchin__SoundControllerWidget,  qt_static_metacall, 0, 0}
};


const QMetaObject *urchin::SoundControllerWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *urchin::SoundControllerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_urchin__SoundControllerWidget.stringdata))
        return static_cast<void*>(const_cast< SoundControllerWidget*>(this));
    if (!strcmp(_clname, "Observer"))
        return static_cast< Observer*>(const_cast< SoundControllerWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int urchin::SoundControllerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
