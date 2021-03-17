#ifndef ABOUTQET_H
	#define ABOUTQET_H
	#include <QtGui>
	/**
	This class represents the dialog box
	About QELECTROTECH �
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
