#include <QApplication>
#include <QTranslator>
#include "qetapp.h"
#include <QtDebug>
#include "debug.h"

/**
Main function of the QElectroTech program
@param argc number of parameters
@param argv parameters
*/
int main(int argc, char **argv) {
	// Creation de l'application
	QApplication app(argc, argv);

	trace_msg("");



	// Translator
	QTranslator trad;
	//trad.load("qet_en");
	app.installTranslator(&trad);
	// Creation and display of the QETApp: QElectroTechApplication
	(new QETApp()) -> show();
	// Execution of the application
	return(app.exec());
}
