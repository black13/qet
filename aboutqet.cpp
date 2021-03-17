#include "aboutqet.h"

/**
Builder
@param parent The parent QWidget of the dialog box
*/
AboutQET::AboutQET(QWidget *parent) : QDialog(parent) {
	// Title, size, behavior ...
	setWindowTitle(tr("\300 propos de QElectrotech"));
	setMinimumWidth(680);
	setMinimumHeight(350);
	setModal(true);
	
    // three tabs
	QTabWidget *onglets = new QTabWidget(this);
	onglets -> addTab(ongletAPropos(), tr("\300 &propos"));
	onglets -> addTab(ongletAuteurs(), tr("A&uteurs"));
	onglets -> addTab(ongletLicence(), tr("&Accord de licence"));
	
	// a button to close the dialog box
	QDialogButtonBox *boutons = new QDialogButtonBox(QDialogButtonBox::Close);
	connect(boutons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(boutons, SIGNAL(rejected()), this, SLOT(accept()));
	
	// all in a vertical provision
	QVBoxLayout *disposition = new QVBoxLayout();
	disposition -> addWidget(titre());
	disposition -> addWidget(onglets);
	disposition -> addWidget(boutons);
	setLayout(disposition);
}

/**
	@return the title QElectroTech with its icon
*/
QWidget *AboutQET::titre() {
	QWidget *icone_et_titre = new QWidget();
	// icons
	QLabel *icone = new QLabel();
	icone -> setPixmap(QIcon(":/ico/qelectrotech.png").pixmap(48, 48));
	// label "QElectroTech"
	QLabel *titre = new QLabel("<span style=\"font-weight:0;font-size:16pt;\">QElectroTech</span>");
	titre -> setTextFormat(Qt::RichText);
	// all in a grid
	QGridLayout *dispo_horiz = new QGridLayout();
	dispo_horiz -> addWidget(icone, 0, 0);
	dispo_horiz -> addWidget(titre, 0, 1);
	dispo_horiz -> setColumnStretch(0, 1);
	dispo_horiz -> setColumnStretch(1, 100);
	icone_et_titre -> setLayout(dispo_horiz);
	return(icone_et_titre);
}

/**
	@return the widget contained by the tab � About
*/
QWidget *AboutQET::ongletAPropos() {
	QLabel *apropos = new QLabel(
		tr(
			"QElectroTech, une application de "
			"r\351alisation de sch\351mas \351lectriques.\n\n\251 2006 Les "
			"d\351veloppeurs de QElectroTech\n\nMerde on n'a pas de site web"
		)
	);
	apropos -> setAlignment(Qt::AlignCenter);
	return(apropos);
}

/**
	@return le widget count by the onglet � auteurs �
*/
QWidget *AboutQET::ongletAuteurs() {
	QLabel *auteurs = new QLabel(
		tr(
			"Id\351e originale : Beno\356t Ansieau <benoit.ansieau@gmail.com>\n\n"
			"Programmation : Xavier Guerrin <xavier.guerrin@gmail.com>"
		)
	);
	auteurs -> setAlignment(Qt::AlignCenter);
	return(auteurs);
}

/**
	@return the widget contained by the tab � License Agreement �
*/
QWidget *AboutQET::ongletLicence() {
	QWidget *licence = new QWidget();
	// label
	QLabel *titre_licence = new QLabel(tr("Ce programme est sous licence GNU/GPL."));
	
	// Recovery of the text of the GNU / GPL in an external file
	QFile *fichier_gpl = new QFile("./gnugpl.txt");
	QString txt_gpl;
	// verify that the file exists
	if (!fichier_gpl -> exists()) {
		txt_gpl = QString(tr("Le fichier texte contenant la licence GNU/GPL est introuvable - bon bah de toute fa\347on, vous la connaissez par coeur non ?"));
	} else {
		// Open the file in text and read-only mode
		if (!fichier_gpl -> open(QIODevice::ReadOnly | QIODevice::Text)) {
			txt_gpl = QString(tr("Le fichier texte contenant la licence GNU/GPL existe mais n'a pas pu \352tre ouvert - bon bah de toute fa\347on, vous la connaissez par coeur non ?"));
		} else {
			// charge the contents of the file in a QString
			QTextStream in(fichier_gpl);
			txt_gpl = QString("");
			while (!in.atEnd()) txt_gpl += in.readLine()+"\n";
			// closes the file
			fichier_gpl -> close();
		}
	}
	
	// Text of the GNU / GPL in a non-editable scrollable text box
	QTextEdit *texte_licence = new QTextEdit();
	texte_licence -> setPlainText(txt_gpl);
	texte_licence -> setReadOnly(true);
	
	// all in a vertical provision
	QVBoxLayout *dispo_licence = new QVBoxLayout();
	dispo_licence -> addWidget(titre_licence);
	dispo_licence -> addWidget(texte_licence);
	licence -> setLayout(dispo_licence);
	return(licence);
}
