#include "panelappareils.h"
#include "contactor.h"
#include "del.h"
#include "entree.h"
#include "elementperso.h"
#include "debug.h"
/**
	Constructeur
	@param parent Le QWidget parent du panel d'appareils
	@todo : definir une classe heritant de QListWidgetItem et automatiser tout ca
*/
PanelAppareils::PanelAppareils(QWidget *parent) :  QListWidget(parent) {
	
	// selection unique
	setSelectionMode(QAbstractItemView::SingleSelection);
	
	// drag'n drop autorise
	setDragEnabled(true);
	setAcceptDrops(false);
	setDropIndicatorShown(false);
	
	// style, mouvement et taille des elements
	setIconSize(QSize(50, 50));
	setMovement(QListView::Free);
	setViewMode(QListView::ListMode);
	
	// donnees
	/*Element *del = new DEL(0,0);
	Element *contacteur = new Contactor(0,0);
	Element *entree = new Entree(0, 0);*/
	
	QListWidgetItem *qlwi;
	QString whats_this = tr("This is a aliment that you can insert  into your diagram schema by clicking and dragging");
	QString tool_tip = tr("Click - drop this aliment on the diagram \ 351ma to insert ");
	
	// remplissage de la liste
	QDir dossier_elements("elements/");
	QStringList filtres;
	filtres << "*.elmt";
	QStringList files = dossier_elements.entryList(filtres, QDir::Files, QDir::Name);
	foreach(QString file, files) {
		int etat;
		ElementPerso *elmt_perso = new ElementPerso(file, 0, 0, &etat);
		if (etat != 0) {
			qDebug() << "Component loading" << file << "failed with error code" << etat;
			continue;
		}
		qlwi = new QListWidgetItem(QIcon(elmt_perso -> pixmap()), elmt_perso -> nom(), this);
		qlwi -> setStatusTip(tool_tip + "\253 " + elmt_perso -> nom() + " \273");
		qlwi -> setToolTip(elmt_perso -> nom());
		qlwi -> setWhatsThis(whats_this);
		qlwi -> setFlags(Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled);
		qlwi -> setData(42, file);
	}
	
	// force du noir sur une alternance de blanc (comme le schema) et de bleu clair
	QPalette qp = palette();
	setAlternatingRowColors(true);
	qp.setColor(QPalette::Text, Qt::black);
	qp.setColor(QPalette::Base, Qt::white);
	//qp.setColor(QPalette::AlternateBase, QColor(240, 255, 255));
	setPalette(qp);
}

/**
Manage movement during a drag'n drop
*/
void PanelAppareils::dragMoveEvent(QDragMoveEvent *) {
}

/**
	Manage the deposit during a drag'n drop
*/
void PanelAppareils::dropEvent(QDropEvent */*e*/) {
}

/**
Manage the start of drag'n drop
@param supportedActions Supported actions
@todo transfer the lines like "if (" such device ") build TelAppareil" => find a way to automate this
 */
void PanelAppareils::startDrag(Qt::DropActions /*supportedActions*/) {
	// objet QDrag pour realiser le drag'n drop
	//qDebug() << "foobar";

	QDrag *drag = new QDrag(this);
	
	// donnees qui seront transmises par le drag'n drop
	QMimeData *mimeData = new QMimeData();
	
	// appareil temporaire pour fournir un apercu
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
	
	// accrochage d'une pixmap representant l'appareil au pointeur
	drag -> setPixmap(appar -> pixmap());
	drag -> setHotSpot(appar -> hotspot());
	
	// realisation du drag'n drop
	drag -> start(Qt::CopyAction);
	
	// suppression de l'appareil temporaire
	delete appar;
}
