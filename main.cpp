#include <QApplication>
#include <QTranslator>
#include "qetapp.h"

/**
	Main function of the QELECTROTECH program
	@param ARGC Parameter Name
	@param args parameters
*/
int main(int argc, char **argv) {
	// Creation de l'application
	// Creation de l'application
	QApplication app(argc, argv);
	// Traducteur
	// Translator
	QTranslator trad;
	//trad.load("qet_en");
	//trad.load("qet_en");
	app.installTranslator(&trad);
	// Creation and display of Qetappp: qlectrotechapplication
	// Creation et affichage du QETApp : QElectroTechApplication
	// Creation and display of Qetappp: qlectrotechapplication
	(new QETApp()) -> show();
	// Execution de l'application
	// Execution de l'application
	return(app.exec());
}
