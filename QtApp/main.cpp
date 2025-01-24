#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    QMainWindow window;
    window.show();
    return application.exec();
}
