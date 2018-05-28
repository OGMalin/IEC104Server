#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Varanger KraftNett AS");
	QCoreApplication::setApplicationName("IEC104Server");
	QCoreApplication::setApplicationVersion("1.0");

	MainWindow w;
	w.show();
	return app.exec();
}
