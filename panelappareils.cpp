#include "panelappareils.h"
#include "contacteur.h"
#include "del.h"
#include "entree.h"
#include "elementperso.h"

/**
	Manufacturer
	@param parent The parent Qwidget of the device panel
	@todo: DEFINE A HERITANT QLISTWIDGETITEM class and automate all that
*/
PanelAppareils::PanelAppareils(QWidget *parent) :  QListWidget(parent) {
	
	// selection unique
	setSelectionMode(QAbstractItemView::SingleSelection);
	
	// Drag'n drop authorizes
	setDragEnabled(true);
	setAcceptDrops(false);
	setDropIndicatorShown(false);
	
	// style, movement and size of the elements
	setIconSize(QSize(50, 50));
	setMovement(QListView::Free);
	setViewMode(QListView::ListMode);
	
	// data
	/* Element * of the = new of (0.0);
	Element * contactor = new contactor (0.0);
	Element *entree = new Entree(0, 0);*/
	
	QListWidgetItem *qlwi;
	QString whats_this = tr("Ceci est un \351l\351ment que vous pouvez ins\351rer dans votre sch\351ma par cliquer-d\351placer");
	QString tool_tip = tr("Cliquer-d\351posez cet \351l\351ment sur le sch\351ma pour ins\351rer un \351l\351ment ");
	
	// fill the list
	QDir dossier_elements("elements/");
	QStringList filtres;
	filtres << "*.elmt";
	QStringList fichiers = dossier_elements.entryList(filtres, QDir::Files, QDir::Name);
	foreach(QString fichier, fichiers) {
		int etat;
		ElementPerso *elmt_perso = new ElementPerso(fichier, 0, 0, &etat);
		if (etat != 0) {
			qDebug() << "Le chargement du composant" << fichier << "a echoue avec le code d'erreur" << etat;
			continue;
		}
		qlwi = new QListWidgetItem(QIcon(elmt_perso -> pixmap()), elmt_perso -> nom(), this);
		qlwi -> setStatusTip(tool_tip + "\253 " + elmt_perso -> nom() + " \273");
		qlwi -> setToolTip(elmt_perso -> nom());
		qlwi -> setWhatsThis(whats_this);
		qlwi -> setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
		qlwi -> setData(42, fichier);
	}
	
	// black force on an alternation of white (like the scheme) and light blue
	QPalette qp = palette();
	setAlternatingRowColors(true);
	qp.setColor(QPalette::Text, Qt::black);
	qp.setColor(QPalette::Base, Qt::white);
	//qp.setColor(QPalette::AlternateBase, QColor(240, 255, 255));
	setPalette(qp);
}

/**
	Gera the movement during a Drag'n Drop
*/
void PanelAppareils::dragMoveEvent(QDragMoveEvent */*e*/) {
}

/**
	GE DEPOSIT DURING DRAG'N DROP
*/
void PanelAppareils::dropEvent(QDropEvent */*e*/) {
}

/**
	GERE THE BEGINNING OF DRAG'N DROP
	@param supported Actions Les actions supporters
	@todo turn the �if type lines ("such device") Build TelApplay => Find a way to automate CA
 */
void PanelAppareils::startDrag(Qt::DropActions /*supportedActions*/) {
	// QDAG object to achieve Drag'n Drop
	QDrag *drag = new QDrag(this);
	
	// data that will be transmitted by Drag'n Drop
	QMimeData *mimeData = new QMimeData();
	
	// temporary device to provide an overview
	Element *appar;
	int etat;
	QString nom_fichier = currentItem() -> data(42).toString();
	appar = new ElementPerso(nom_fichier, 0, 0, &etat);
	if (etat != 0) {
		delete appar;
		return;
	}
	
	mimeData -> setText(nom_fichier);
	drag -> setMimeData(mimeData);
	
	// attach a pixmap representing the device to the pointer
	drag -> setPixmap(appar -> pixmap());
	drag -> setHotSpot(appar -> hotspot());
	
	// Realization You Drag'n Drop
	drag -> start(Qt::CopyAction);
	
	// Deleting the temporary device
	delete appar;
}
