#include "tabbable_main_window.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    TabbableMainWindow tabbable_main_window;
    tabbable_main_window.showMaximized();
    return QApplication::exec();
}
