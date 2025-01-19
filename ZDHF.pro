TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG += qt

QT += core gui widgets

SOURCES += \
        main.cpp \
        pieceofimage.cpp \
        createalpha.cpp \
        tools.cpp \
        imagepuzzle.cpp

HEADERS += \
        pieceofimage.h \
        tools.H \
        createalpha.h \
        imagepuzzle.h

# 启用自动moc
CONFIG += automoc

INCLUDEPATH += /opt/homebrew/opt/opencv/include/opencv4
INCLUDEPATH += /opt/homebrew/opt/opencv/include/opencv4/opencv2

# 设置库目录
LIB_DIR = /opt/homebrew/opt/opencv/lib
LIBS += -L$$LIB_DIR -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_imgproc -lopencv_photo
