#pragma once

#include "Database.h"
#include <QWidget>

class SNMPWindow : public QWidget
{
	Q_OBJECT

public:
	Database* database;
	SNMPWindow(Database* db);
};
