/****************************************************************************
** Meta object code from reading C++ file 'MapEditorWindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.2.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../src/MapEditorWindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MapEditorWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.2.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_urchin__MapEditorWindow_t {
    QByteArrayData data[9];
    char stringdata[161];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    offsetof(qt_meta_stringdata_urchin__MapEditorWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData) \
    )
static const qt_meta_stringdata_urchin__MapEditorWindow_t qt_meta_stringdata_urchin__MapEditorWindow = {
    {
QT_MOC_LITERAL(0, 0, 23),
QT_MOC_LITERAL(1, 24, 13),
QT_MOC_LITERAL(2, 38, 0),
QT_MOC_LITERAL(3, 39, 14),
QT_MOC_LITERAL(4, 54, 17),
QT_MOC_LITERAL(5, 72, 16),
QT_MOC_LITERAL(6, 89, 18),
QT_MOC_LITERAL(7, 108, 17),
QT_MOC_LITERAL(8, 126, 33)
    },
    "urchin::MapEditorWindow\0showNewDialog\0"
    "\0showOpenDialog\0executeSaveAction\0"
    "showSaveAsDialog\0executeCloseAction\0"
    "executeExitAction\0executeViewPropertiesChangeAction\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_urchin__MapEditorWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x08,
       3,    0,   50,    2, 0x08,
       4,    0,   51,    2, 0x08,
       5,    0,   52,    2, 0x08,
       6,    0,   53,    2, 0x08,
       7,    0,   54,    2, 0x08,
       8,    0,   55,    2, 0x08,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void urchin::MapEditorWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MapEditorWindow *_t = static_cast<MapEditorWindow *>(_o);
        switch (_id) {
        case 0: _t->showNewDialog(); break;
        case 1: _t->showOpenDialog(); break;
        case 2: _t->executeSaveAction(); break;
        case 3: _t->showSaveAsDialog(); break;
        case 4: { bool _r = _t->executeCloseAction();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 5: _t->executeExitAction(); break;
        case 6: _t->executeViewPropertiesChangeAction(); break;
        default: ;
        }
    }
}

const QMetaObject urchin::MapEditorWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_urchin__MapEditorWindow.data,
      qt_meta_data_urchin__MapEditorWindow,  qt_static_metacall, 0, 0}
};


const QMetaObject *urchin::MapEditorWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *urchin::MapEditorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_urchin__MapEditorWindow.stringdata))
        return static_cast<void*>(const_cast< MapEditorWindow*>(this));
    if (!strcmp(_clname, "Observer"))
        return static_cast< Observer*>(const_cast< MapEditorWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int urchin::MapEditorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
