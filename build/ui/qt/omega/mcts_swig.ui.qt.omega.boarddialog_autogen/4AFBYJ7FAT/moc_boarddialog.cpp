/****************************************************************************
** Meta object code from reading C++ file 'boarddialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../ui/qt/omega/include/ui/qt/omega/boarddialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'boarddialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_BoardDialog_t {
    QByteArrayData data[11];
    char stringdata0[146];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_BoardDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_BoardDialog_t qt_meta_stringdata_BoardDialog = {
    {
QT_MOC_LITERAL(0, 0, 11), // "BoardDialog"
QT_MOC_LITERAL(1, 12, 12), // "back_to_main"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 21), // "on_quitButton_clicked"
QT_MOC_LITERAL(4, 48, 22), // "on_startButton_clicked"
QT_MOC_LITERAL(5, 71, 15), // "updateCountdown"
QT_MOC_LITERAL(6, 87, 15), // "updateGameState"
QT_MOC_LITERAL(7, 103, 12), // "Omega::Piece"
QT_MOC_LITERAL(8, 116, 5), // "piece"
QT_MOC_LITERAL(9, 122, 7), // "cellIdx"
QT_MOC_LITERAL(10, 130, 15) // "updateFromAiBot"

    },
    "BoardDialog\0back_to_main\0\0"
    "on_quitButton_clicked\0on_startButton_clicked\0"
    "updateCountdown\0updateGameState\0"
    "Omega::Piece\0piece\0cellIdx\0updateFromAiBot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_BoardDialog[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   45,    2, 0x08 /* Private */,
       4,    0,   46,    2, 0x08 /* Private */,
       5,    0,   47,    2, 0x08 /* Private */,
       6,    2,   48,    2, 0x0a /* Public */,
      10,    0,   53,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7, QMetaType::UInt,    8,    9,
    QMetaType::Void,

       0        // eod
};

void BoardDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        BoardDialog *_t = static_cast<BoardDialog *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->back_to_main(); break;
        case 1: _t->on_quitButton_clicked(); break;
        case 2: _t->on_startButton_clicked(); break;
        case 3: _t->updateCountdown(); break;
        case 4: _t->updateGameState((*reinterpret_cast< Omega::Piece(*)>(_a[1])),(*reinterpret_cast< uint(*)>(_a[2]))); break;
        case 5: _t->updateFromAiBot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            typedef void (BoardDialog::*_t)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&BoardDialog::back_to_main)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject BoardDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_BoardDialog.data,
      qt_meta_data_BoardDialog,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *BoardDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *BoardDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_BoardDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int BoardDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void BoardDialog::back_to_main()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
