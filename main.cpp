#include <QApplication>
#include <QStyleFactory>
#include <QSettings>
#include <QPalette>
#include "imagepuzzle.h"

bool isDarkMode() {
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", QSettings::NativeFormat);
    return settings.value("AppsUseLightTheme", 1).toInt() == 0;
}

void setDarkTheme(QApplication &app) {
    app.setStyle(QStyleFactory::create("Fusion"));
    
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(35,35,35));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Highlight, QColor(42,130,218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    app.setPalette(darkPalette);
    
    QString styleSheet = R"(
        QToolTip {
            color: #ffffff;
            background-color: #2a82da;
            border: 1px solid white;
        }
        QToolBar {
            background-color: #353535;
            border: 1px solid #404040;
        }
        QToolButton {
            background-color: transparent;
            color: white;
            padding: 5px;
        }
        QToolButton:hover {
            background-color: #404040;
        }
        QLabel {
            color: white;
        }
    )";
    
    app.setStyleSheet(styleSheet);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // 检测并设置暗黑模式
    if(isDarkMode()) {
        setDarkTheme(app);
    }

    ImagePuzzle window;
    window.show();

    return app.exec();
}
