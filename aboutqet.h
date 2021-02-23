#ifndef ABOUTQET_H
	#define ABOUTQET_H
	//#include <QtGui>
	#include <QtWidgets>
	/**
		This class represents the dialog box
		"About QElectroTech"
	*/
	class AboutQET : public QDialog {
		Q_OBJECT
		public:
		AboutQET(QWidget * = 0);
		private:
		QWidget *titre();
		QWidget *ongletAPropos();
		QWidget *ongletAuteurs();
		QWidget *ongletLicence();
	};
#endif
