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
else:unix: LIBS += -L$$PWD/lib/ -lQGLViewer

win32:INCLUDEPATH += $$PWD/include
win32:DEPENDPATH += $$PWD/include


# Assimp
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lassimp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lassimpd
else:unix: LIBS += -L$$PWD/lib/ -lassimp

unix:QMAKE_CXXFLAGS += -std=c++11


# Glew
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lglew32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lglew32d
else:unix: LIBS += -L$$PWD/lib/ -lGLEW -lGLU


PRO_PATH=$$PWD
DEFINES += "MODEL_PATH=\"\\\"$$PRO_PATH/model/\\\"\""
