#pragma once

#include <QMainWindow>
#include <QTranslator>

class QMenu;
class QAction;
class QEvent;
class QCloseEvent;
class QActionGroup;
class QTabWidget;
class StatusWindow;
class IECWindow;
class SNMPWindow;
class IEC104Slave;
class Database;

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	QMenu * fileMenu;
	QMenu* settingsMenu;
	QMenu* helpMenu;
	QMenu* langMenu;
	QToolBar* toolbar;
	QAction* exitAct;
	QAction* engAct;
	QAction* norAct;
	QAction* aboutAct;
	QTranslator translator;
	QActionGroup * langGroup;
	QString locale;
	QTabWidget* tabbar;
	StatusWindow* statuswindow;
	IECWindow* iecwindow;
	SNMPWindow* snmpwindow;
	IEC104Slave* iec104;
	Database* db;
	void closeEvent(QCloseEvent* event) override;
	void createMenu();
	void loadLanguage();
	void retranslateUi();
	void readSettings();
	void writeSettings();

protected:
	void changeEvent(QEvent* event);

protected slots:
	void slotLanguageChanged(QAction* action);

public:
	MainWindow();
	virtual ~MainWindow();
};