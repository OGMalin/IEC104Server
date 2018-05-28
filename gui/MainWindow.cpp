#include <QMainWindow>
#include <QtWidgets>
#include <QMenu>
#include <QAction>
#include <QEvent>
#include <QCloseEvent>
#include <QActionGroup>
#include <QTabWidget>
#include "MainWindow.h"
#include "StatusWindow.h"
#include "IECWindow.h"
#include "SNMPWindow.h"

MainWindow::MainWindow()
{
	readSettings();
	createMenu();
	statusBar();
	loadLanguage();

	tabbar = new QTabWidget();
	statuswindow = new StatusWindow();
	iecwindow = new IECWindow();
	snmpwindow = new SNMPWindow();

	tabbar->addTab(statuswindow, "Status");
	tabbar->addTab(iecwindow, "IEC");
	tabbar->addTab(snmpwindow, "SNMP");
	
	setCentralWidget(tabbar);
	retranslateUi();
}

MainWindow::~MainWindow()
{

}

// The text in the menu are set in retranslateUi to be able to switch language 'on the fly'.
void MainWindow::createMenu()
{
	//Main menues

	// File menu
	fileMenu = menuBar()->addMenu("*");
	exitAct = fileMenu->addAction("*", this, &QWidget::close);


	// Settings menu
	settingsMenu = menuBar()->addMenu("*");
	//	fileMenu->addSeparator();
	langMenu = settingsMenu->addMenu("*");
	engAct = langMenu->addAction(QIcon(":/icon/GB.png"), "*");
	engAct->setCheckable(true);
	engAct->setData("gb");
	norAct = langMenu->addAction(QIcon(":/icon/NO.png"), "*");
	norAct->setCheckable(true);
	norAct->setData("nb");
	langGroup = new QActionGroup(this);
	connect(langGroup, SIGNAL(triggered(QAction *)), this, SLOT(slotLanguageChanged(QAction *)));
	langGroup->addAction(engAct);
	langGroup->addAction(norAct);
	if (locale == "nb")
		norAct->setChecked(true);
	else
		engAct->setChecked(true);

	// Help menu
	helpMenu = menuBar()->addMenu("*");
	aboutAct = helpMenu->addAction("*");

	// Setting up the toolbar
//	toolbar = addToolBar("Toolbar");
}

void MainWindow::retranslateUi()
{
	fileMenu->setTitle(tr("File"));
	exitAct->setText(tr("Exit"));

	settingsMenu->setTitle(tr("Settings"));
	langMenu->setTitle(tr("Language"));
	if (locale == "nb")
		langMenu->setIcon(QIcon(":/icon/NO.png"));
	else
		langMenu->setIcon(QIcon(":/icon/GB.png"));
	engAct->setText(tr("English"));
	norAct->setText(tr("Norwegian"));
	helpMenu->setTitle(tr("Help"));
	aboutAct->setText(tr("About..."));
}

void MainWindow::slotLanguageChanged(QAction* action)
{
	if (0 != action) {
		// load the language dependant on the action content
		locale = action->data().toString();
		loadLanguage();
	}
}

void MainWindow::loadLanguage()
{
	if (locale == "nb")
	{
		if (translator.isEmpty())
		{
			if (translator.load(":/language/iec104server_nb.qm"))
				qApp->installTranslator(&translator);
		}
		else
		{
			qApp->installTranslator(&translator);
		}
		return;
	}

	if (!translator.isEmpty())
		qApp->removeTranslator(&translator);
	return;
}

void MainWindow::changeEvent(QEvent* event)
{
	if (0 != event) {
		switch (event->type()) {
			// this event is send if a translator is loaded
		case QEvent::LanguageChange:
			retranslateUi();
			break;

			// this event is send, if the system, language changes
		case QEvent::LocaleChange:
		{
			locale = QLocale::system().name();
			locale.truncate(locale.lastIndexOf('_'));
			loadLanguage();
		}
		break;
		}
	}
	QMainWindow::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	writeSettings();
	event->accept();
}

void MainWindow::writeSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("maingeometry", saveGeometry());
	//	settings.setValue("hgeometry", hSplitter->saveState());
	//	settings.setValue("vgeometry", vSplitter->saveState());
	settings.setValue("language", locale);
}

void MainWindow::readSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	QByteArray maingeometry = settings.value("maingeometry", QByteArray()).toByteArray();
	//	QByteArray hgeometry = settings.value("hgeometry", QByteArray()).toByteArray();
	//	QByteArray vgeometry = settings.value("vgeometry", QByteArray()).toByteArray();
	locale = settings.value("language", QString()).toString();
	if (maingeometry.isEmpty())
	{
		const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
		resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
		move((availableGeometry.width() - width()) / 2,
			(availableGeometry.height() - height()) / 2);
	}
	else
	{
		restoreGeometry(maingeometry);
	}
	/* Problem with read access
	if (!hgeometry.isEmpty())
	hSplitter->restoreState(hgeometry);
	if (!vgeometry.isEmpty())
	vSplitter->restoreState(vgeometry);
	*/
	if (locale.isEmpty())
	{
		// Find the systems default language
		locale = QLocale::system().name();
		locale.truncate(locale.lastIndexOf('_'));
	}
	else
	{
		locale = settings.value("language", QString("gb")).toString();
	}
}
