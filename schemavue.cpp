#include "schemavue.h"
#include "schema.h"
#include "elementperso.h"
#include "contacteur.h"
#include "del.h"
#include "entree.h"

/**
	Initializes the schemavue
*/
void SchemaVue::initialise() {
	setInteractive(true);
	setAntialiasing(true);
	setScene(scene = new Schema(this));
	setDragMode(RubberBandDrag);
	setAcceptDrops(true);
	setWindowTitle(tr("Nouveau sch\351ma") + "[*]");
	connect(scene, SIGNAL(selectionChanged()), this, SLOT(slot_selectionChanged()));
}

/**
	Default builder
*/
SchemaVue::SchemaVue() : QGraphicsView() {
	initialise();
}

/**
	Manufacturer
	@param parent the parent qwidegt of this scheme view
*/
SchemaVue::SchemaVue(QWidget *parent) : QGraphicsView(parent) {
	initialise();
}

/**
	Lets you know if the graphic rendering of the schemavue is antialiasis or not.
	@return a booleen indicating whether the schemavue is antialiasis
*/
bool SchemaVue::antialiased() const {
	return(antialiasing);
}

/**
	Active or disable the antialiasing for the graphic rendering of the schemavue.
	@Param has a booleen indicating whether the scheme view must be antialiased or not
*/
void SchemaVue::setAntialiasing(bool aa) {
	antialiasing = aa;
	setRenderHint(QPainter::Antialiasing, aa);
	setRenderHint(QPainter::TextAntialiasing, aa);
	setRenderHint(QPainter::SmoothPixmapTransform, aa);
	repaint();
}

/**
	calls the SELECT method on all elements of the list of elements
	@todo Edit Selectall for Integration of Drivers
*/
void SchemaVue::selectAll() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(true);
}

/**
	Calls the Delect method on all elements of the list of elements
	@todo modify selectnothing for the integration of drivers
*/
void SchemaVue::selectNothing() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(false);
}

/**
	Reverses the selection state of all elements of the list of elements
	@todo Edit Selectinvert for Integration of Drivers
 */
void SchemaVue::selectInvert() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(!item -> isSelected());
}

/**
	Deletes the selection components
*/
void SchemaVue::supprimer() {
	QList<QGraphicsItem *> garbage_elmt;
	QList<QGraphicsItem *>   garbage_conducteurs;
	
	// useless but careful : creating two lists : one for wires, one for elements
	foreach (QGraphicsItem *qgi, scene -> selectedItems()) {
		if (!garbage_elmt.contains(qgi)) garbage_elmt.append(qgi);
		// For each child of the element
		foreach (QGraphicsItem *child, qgi -> children()) {
			// if this child is a terminal
			if (Borne *p = qgraphicsitem_cast<Borne *>(child)) {
				// then each driver of the terminal is listed
				foreach (Conducteur *f, p -> conducteurs()) {
					if (!garbage_conducteurs.contains(f)) garbage_conducteurs.append(f);
				}
			}
		}
	}
	scene -> clearSelection();
	
	// "destroying" the wires, removing them from the scene and stocking them into the � garbage �
	foreach (QGraphicsItem *qgi, garbage_conducteurs) {
		if (Conducteur *f = qgraphicsitem_cast<Conducteur *>(qgi)) {
			f -> destroy();
			scene -> removeItem(f);
			throwToGarbage(f);
		}
	}
	
	// removing the elements from the scene and stocking them into the � garbage �
	foreach (QGraphicsItem *qgi, garbage_elmt) {
		scene -> removeItem(qgi);
		throwToGarbage(qgi);
	}
	resetCachedContent();
	QTimer::singleShot(5000, this, SLOT(flushGarbage()));
}

/**
	Send an item to the "garbage" so that it is deleted later
	@param qgi L'item a supprimer
*/
void SchemaVue::throwToGarbage(QGraphicsItem *qgi) {
	// No duplicate in the garbage (otherwise it will feel the segfault)
	bool qgi_deja_dans_le_garbage = false;
	foreach(QGraphicsItem *gbg_qgi, garbage) {
		if ((void *)gbg_qgi == (void *)qgi) {
			qgi_deja_dans_le_garbage = true;
			break;
		}
	}
	if (!qgi_deja_dans_le_garbage) garbage.append(qgi);
}

/**
	Deletes all the elements of the "garbage"
*/
void SchemaVue::flushGarbage() {
	foreach(QGraphicsItem *qgi, garbage) {
		delete(qgi);
		garbage.removeAll(qgi);
	}
}

/**
	Pivot the selection components
*/
void SchemaVue::pivoter() {
	if (scene -> selectedItems().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> selectedItems()) {
		if (Element *elt = qgraphicsitem_cast<Element *>(item)) {
			elt -> invertOrientation();
			elt -> update();
		}
	}
}

/**
	Accepts or refuses Drag'n Drop according to the type of incoming data
	@param e the QDragénterevent corresponding to the Drag'n Drop tent
	@todo Find a mime more adaptive
*/
void SchemaVue::dragEnterEvent(QDragEnterEvent *e) {
	if (e -> mimeData() -> hasFormat("text/plain")) e -> acceptProposedAction();
	else e-> ignore();
}

/**
	gere les dragleaveevent
	@param e the QDragénterevent corresponding to the Drag'n Drop outgoing
*/
void SchemaVue::dragLeaveEvent(QDragLeaveEvent *) {}

/**
	Accepts or refuses Drag'n Drop according to the type of incoming data
	@param e the QDragmoveEvent corresponding to the Drag'n Drop tent
*/
void SchemaVue::dragMoveEvent(QDragMoveEvent *e) {
	if (e -> mimeData() -> hasFormat("text/plain")) e -> acceptProposedAction();
	else e-> ignore();
}

/**
	GERE THE DEPOSITS (DROP) ACCEPTED ON THE SCHEMA
	@param e the QDRopevent corresponding to the Drag'n Drop carries out
	@todo add directly the object to the scene when the Drag'n Drop will have been improved
*/
void SchemaVue::dropEvent(QDropEvent *e) {
	QString fichier = e -> mimeData() -> text();
	int etat;
	Element *el = new ElementPerso(fichier, 0, 0, &etat);
	if (etat != 0) delete el;
	else {
		scene -> addItem(el);
		el -> setPos(mapToScene(e -> pos().x(), e -> pos().y()));
		el -> setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	}
}

/**
	Pass the schema in visualization mode
*/
void SchemaVue::setVisualisationMode() {
	setDragMode(ScrollHandDrag);
	emit(modeChanged());
}

/**
	Pass the schema in Selection mode
*/
void SchemaVue::setSelectionMode() {
	setDragMode(RubberBandDrag);
	setCursor(Qt::ArrowCursor);
	emit(modeChanged());
}

/**
	Enlarge the schema (+ 33% = reverse of -25% zomons ())
*/
void SchemaVue::zoomPlus() {
	scale(4.0/3.0, 4.0/3.0);
}

/**
	Retreciate the schema (-25% = reverse of +33% of Zoomomlus ())
*/
void SchemaVue::zoomMoins() {
	scale(0.75, 0.75);
}

/**
	Enlarge or rectified the schema in fact to what all the elements of the
	Schema is visible to the screen.If there is no element on the schema,
	The zoom is reinitialized
*/
void SchemaVue::zoomFit() {
	if (scene -> items().isEmpty()) {
		zoomReset();
		return;
	}
	QRectF vue = scene -> itemsBoundingRect();
	// la marge  = 5 % de la longueur necessaire
	qreal marge = 0.05 * vue.width();
	vue.translate(-marge, -marge);
	vue.setWidth(vue.width() + 2.0 * marge);
	vue.setHeight(vue.height() + 2.0 * marge);
	fitInView(vue, Qt::KeepAspectRatio);
}

/**
	Reset zoom
*/
void SchemaVue::zoomReset() {
	resetMatrix();
}

/**
Copy the selected elements of the scheme in the clipboard and then removes them
*/
void SchemaVue::couper() {
	copier();
	supprimer();
}

/**
Copy the selection elements of the scheme in the clipboard
*/
void SchemaVue::copier() {
	QClipboard *presse_papier = QApplication::clipboard();
	QString contenu_presse_papier = scene -> toXml(false).toString(4);
	if (presse_papier -> supportsSelection()) presse_papier -> setText(contenu_presse_papier, QClipboard::Selection);
	presse_papier -> setText(contenu_presse_papier);
}

/**
imports the elements contained in the clipboard in the scheme
*/
void SchemaVue::coller() {
	QString texte_presse_papier;
	QDomDocument document_xml;
	if ((texte_presse_papier = QApplication::clipboard() -> text()) == QString()) return;
	if (!document_xml.setContent(texte_presse_papier)) return;
	scene -> fromXml(document_xml);
}

/**
Gera clicks and more especially the middle click (= Paste for x11)
*/
void SchemaVue::mousePressEvent(QMouseEvent *e) {
	if (e -> buttons() == Qt::MidButton) {
		QString texte_presse_papier;
		QDomDocument document_xml;
		if ((texte_presse_papier = QApplication::clipboard() -> text(QClipboard::Selection)) == QString()) return;
		if (!document_xml.setContent(texte_presse_papier)) return;
		scene -> fromXml(document_xml, mapToScene(e -> pos()));
	}
	QGraphicsView::mousePressEvent(e);
}

/**
Opens a * .QET file in this schemavue
@param file_name file name to open
@param error if the pointer is specified, this integer is set to 0 in case of success of the opening, 1 if the file does not exist, 2 if the file is not readable, 3 if the file is notnot an XML element, 4 if the opening of the file has echoue for another reason (it's not that miss ^^)
@return true if the opening has managed, false otherwise
*/
bool SchemaVue::ouvrir(QString n_fichier, int *erreur) {
	// check the existence of the file
	if (!QFileInfo(n_fichier).exists()) {
		if (erreur != NULL) *erreur = 1;
		return(false);
	}
	
	// Open the file
	QFile fichier(n_fichier);
	if (!fichier.open(QIODevice::ReadOnly)) {
		if (erreur != NULL) *erreur = 2;
		return(false);
	}
	
	// reads its contents in a QdomDocument
	QDomDocument document;
	if (!document.setContent(&fichier)) {
		if (erreur != NULL) *erreur = 3;
		fichier.close();
		return(false);
	}
	fichier.close();
	
	// Construct the Scheme The QomDoComent Party
	QDomDocument &doc = document;
	if (scene -> fromXml(doc)) {
		if (erreur != NULL) *erreur = 0;
		nom_fichier = n_fichier;
		setWindowTitle(nom_fichier + "[*]");
		return(true);
	} else {
		if (erreur != NULL) *erreur = 4;
		return(false);
	}
}

void SchemaVue::slot_selectionChanged() {
	emit(selectionChanged());
}

void SchemaVue::closeEvent(QCloseEvent *event) {
	// ask the user first if he wants to register the current schema
	QMessageBox::StandardButton reponse = QMessageBox::question(
		this,
		tr("Enregistrer le sch\351ma en cours ?"),
		tr("Voulez-vous enregistrer le sch\351ma en cours ?"),
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
		QMessageBox::Cancel
	);
	bool retour;

	switch(reponse) {
	case QMessageBox::Cancel: retour = false;         break; // l'utilisateur annule : echec de la fermeture
	case QMessageBox::Yes:    retour = enregistrer(); break; // l'utilisateur dit oui : la reussite depend de l'enregistrement
	default:                  retour = true;  			break;// the user cancels: Failed closure
	}
	if (retour) event -> accept();
	else event -> ignore();
}

/**
	Method recording the schema in the last known file name.
	If no file name is known, this method calls the register_sous method
	@return True if the re-registrationming, FALSE SINON
*/
bool SchemaVue::enregistrer() {
	if (nom_fichier == QString()) return(enregistrer_sous());
	else return(private_enregistrer(nom_fichier));
}

/**
	This method requires a file name to the user to save the schema
	If no name is between, it returns false.
	If the name does not end with the .qet extension, it is added.
	If the successful recording, the file name is kept and the function returns True.
	Otherwise, false is returned.
	@return True if the re-registrationming, FALSE SINON
	@todo Detect the office path automatically
*/
bool SchemaVue::enregistrer_sous() {
	// demande un nom de fichier a l'utilisateur pour enregistrer le schema
	QString n_fichier = QFileDialog::getSaveFileName(
		this,
		tr("Enregistrer sous"),
		QDir::homePath(),
		tr("Schema QelectroTech (*.qet)")
	);
	// If no name is between, returns false.
	if (n_fichier == "") return(false);
	// if the name does not end with the extension .qet, it is added
	if (!n_fichier.endsWith(".qet", Qt::CaseInsensitive)) n_fichier += ".qet";
	// tries to save the file
	bool resultat_enregistrement = private_enregistrer(n_fichier);
	// if the successful recording, the file name is retained
	if (resultat_enregistrement) {
		nom_fichier = n_fichier;
		setWindowTitle(nom_fichier + "[*]");
	}
	// retourne un booleen representatif de la reussite de l'enregistrement
	// returns a booleen representative of the success of the recording
	return(resultat_enregistrement);
}

/**
	PRIVATE METHOD Manage the recording of the XML file.If it is not possible
	To write in the file, this function displays an error message and returns false.
	Otherwise, she returns true.
	@param file_name file name in which the XML shaft must be written
	@return True if the re-registrationming, FALSE SINON
*/
bool SchemaVue::private_enregistrer(QString &n_fichier) {
	QFile fichier(n_fichier);
	if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(this, tr("Erreur"), tr("Impossible d'ecrire dans ce fichier"));
		return(false);
	}
	QTextStream out(&fichier);
	out.setCodec("UTF-8");
	out << scene -> toXml().toString(4);
	fichier.close();
	return(true);
}

