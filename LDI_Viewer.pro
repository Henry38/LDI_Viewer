HEADERS += \
    StandardCamera.h \
    Viewer.h \
    Fragment.h \
    Mesh.h \
    Contact.h \
    Cellule.h

SOURCES += \
    StandardCamera.cpp \
    Viewer.cpp \
    Fragment.cpp \
    main.cpp \
    Mesh.cpp \
    Contact.cpp \
    Cellule.cpp

QT *= xml opengl



# QGLViewer
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lQGLViewer2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lQGLViewer2d
else:unix: LIBS += -L$$PWD/lib/ -lQGLViewer2

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include


# Assimp
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lassimp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lassimpd
else:unix: LIBS += -L$$PWD/lib/ -lassimp

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include


# Glew
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lglew32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lglew32d
else:unix: LIBS += -L$$PWD/lib/ -lglew32

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include
