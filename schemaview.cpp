#include "schemaview.h"
#include "schema.h"
#include "elementperso.h"
#include "contactor.h"
#include "del.h"
#include "entree.h"

/**
	Initialise le SchemaView
*/
void SchemaView::initialise() {

	setInteractive(true);
	setAntialiasing(true);
	setScene(scene = new Schema(this));
	setDragMode(RubberBandDrag);
	setAcceptDrops(true);
	setWindowTitle(tr("New schema") + "[*]");
	connect(scene, SIGNAL(selectionChanged()), this, SLOT(slot_selectionChanged()));
}

/**
	Constructeur par defaut
*/
SchemaView::SchemaView() : QGraphicsView() {
	initialise();
}

/**
	Constructeur
	@param parent Le QWidegt parent de cette vue de schema
*/
SchemaView::SchemaView(QWidget *parent) : QGraphicsView(parent) {
	initialise();
}

/**
	Permet de savoir si le rendu graphique du SchemaView est antialiase ou non.
	@return Un booleen indiquant si le SchemaView est antialiase
*/
bool SchemaView::antialiased() const {
	return(antialiasing);
}

/**
	Active ou desactive l'antialiasing pour le rendu graphique du SchemaView.
	@param aa un booleen indiquant si le SchemaView doit etre antialiase ou non
*/
void SchemaView::setAntialiasing(bool aa) {
	antialiasing = aa;
	setRenderHint(QPainter::Antialiasing, aa);
	setRenderHint(QPainter::TextAntialiasing, aa);
	setRenderHint(QPainter::SmoothPixmapTransform, aa);
	repaint();
}

/**
	appelle la methode select sur tous les elements de la liste d'elements
	@todo modifier selectAll pour l'integration des conducteurs
*/
void SchemaView::selectAll() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(true);
}

/**
	appelle la methode deselect sur tous les elements de la liste d'elements
	@todo modifier selectNothing pour l'integration des conducteurs
*/
void SchemaView::selectNothing() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(false);
}

/**
	Inverse l'etat de selection de tous les elements de la liste d'elements
	@todo modifier selectInvert pour l'integration des conducteurs
 */
void SchemaView::selectInvert() {
	if (scene -> items().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> items()) item -> setSelected(!item -> isSelected());
}

/**
	Supprime les composants selectionnes
*/
void SchemaView::supprimer() {
	QList<QGraphicsItem *> garbage_elmt;
	QList<QGraphicsItem *>   garbage_conducteurs;
	
	// useless but careful : creating two lists : one for wires, one for elements
	foreach (QGraphicsItem *qgi, scene -> selectedItems()) {
		if (!garbage_elmt.contains(qgi)) garbage_elmt.append(qgi);
		// pour chaque enfant de l'element
		foreach (QGraphicsItem *child, qgi -> childItems()) {
			// si cet enfant est une borne
			if (Terminal *p = qgraphicsitem_cast<Terminal *>(child)) {
				// alors chaque conducteur de la borne est recense
				foreach (Conductor *f, p -> conducteurs()) {
					if (!garbage_conducteurs.contains(f)) garbage_conducteurs.append(f);
				}
			}
		}
	}
	scene -> clearSelection();
	
	// "destroying" the wires, removing them from the scene and stocking them into the « garbage »
	foreach (QGraphicsItem *qgi, garbage_conducteurs) {
		if (Conductor *f = qgraphicsitem_cast<Conductor *>(qgi)) {
			f -> destroy();
			scene -> removeItem(f);
			throwToGarbage(f);
		}
	}
	
	// removing the elements from the scene and stocking them into the « garbage »
	foreach (QGraphicsItem *qgi, garbage_elmt) {
		scene -> removeItem(qgi);
		throwToGarbage(qgi);
	}
	resetCachedContent();
	QTimer::singleShot(5000, this, SLOT(flushGarbage()));
}

/**
	Envoie un item vers le "garbage" pour qu'il soit supprime plus tard
	@param qgi L'item a supprimer
*/
void SchemaView::throwToGarbage(QGraphicsItem *qgi) {
	// pas de doublon dans le garbage (sinon ca va sentir la segfault)
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
	Supprime tous les elements du "garbage"
*/
void SchemaView::flushGarbage() {
	foreach(QGraphicsItem *qgi, garbage) {
		delete(qgi);
		garbage.removeAll(qgi);
	}
}

/**
	Pivote les composants selectionnes
*/
void SchemaView::pivoter() {
	if (scene -> selectedItems().isEmpty()) return;
	foreach (QGraphicsItem *item, scene -> selectedItems()) {
		if (Element *elt = qgraphicsitem_cast<Element *>(item)) {
			elt -> invertOrientation();
			elt -> update();
		}
	}
}

/**
	accepte ou refuse le drag'n drop en fonction du type de donnees entrant
	@param e le QDragEnterEvent correspondant au drag'n drop tente
	@todo trouver un MIME Type plus adapte
*/
void SchemaView::dragEnterEvent(QDragEnterEvent *e) {
	if (e -> mimeData() -> hasFormat("text/plain")) e -> acceptProposedAction();
	else e-> ignore();
}

/**
	gere les dragleaveevent
	@param e le QDragEnterEvent correspondant au drag'n drop sortant
*/
void SchemaView::dragLeaveEvent(QDragLeaveEvent *) {}

/**
	accepte ou refuse le drag'n drop en fonction du type de donnees entrant
	@param e le QDragMoveEvent correspondant au drag'n drop tente
*/
void SchemaView::dragMoveEvent(QDragMoveEvent *e) {
	if (e -> mimeData() -> hasFormat("text/plain")) e -> acceptProposedAction();
	else e-> ignore();
}

/**
	gere les depots (drop) acceptes sur le Schema
	@param e le QDropEvent correspondant au drag'n drop effectue
	@todo Ajouter directement l'objet Element a la scene lorsque le drag'n drop aura ete ameliore
*/
void SchemaView::dropEvent(QDropEvent *e) {
	QString file = e -> mimeData() -> text();
	int etat;
	Element *el = new ElementPerso(file, 0, 0, &etat);
	if (etat != 0) delete el;
	else {
		scene -> addItem(el);
		el -> setPos(mapToScene(e -> pos().x(), e -> pos().y()));
		el -> setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
	}
}

/**
	Passe le Schema en mode visualisation
*/
void SchemaView::setVisualisationMode() {
	setDragMode(ScrollHandDrag);
	emit(modeChanged());
}

/**
	Passe le Schema en mode Selection
*/
void SchemaView::setSelectionMode() {
	setDragMode(RubberBandDrag);
	setCursor(Qt::ArrowCursor);
	emit(modeChanged());
}

/**
	Agrandit le schema (+33% = inverse des -25 % de zoomMoins())
*/
void SchemaView::zoomPlus() {
	scale(4.0/3.0, 4.0/3.0);
}

/**
	Retrecit le schema (-25% = inverse des +33 % de zoomPlus())
*/
void SchemaView::zoomMoins() {
	scale(0.75, 0.75);
}

/**
	Agrandit ou rectrecit le schema de facon a ce que tous les elements du
	schema soient visibles a l'ecran. S'il n'y a aucun element sur le schema,
	le zoom est reinitialise
*/
void SchemaView::zoomFit() {
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
	Reinitialise le zoom
*/
void SchemaView::zoomReset() {
	resetMatrix();
}

/**
	copie les elements selectionnes du schema dans le presse-papier puis les supprime
*/
void SchemaView::couper() {
	copier();
	supprimer();
}

/**
	copie les elements selectionnes du schema dans le presse-papier
*/
void SchemaView::copier() {
	QClipboard *presse_papier = QApplication::clipboard();
	QString contenu_presse_papier = scene -> toXml(false).toString(4);
	if (presse_papier -> supportsSelection()) presse_papier -> setText(contenu_presse_papier, QClipboard::Selection);
	presse_papier -> setText(contenu_presse_papier);
}

/**
	importe les elements contenus dans le presse-papier dans le schema
*/
void SchemaView::coller() {
	QString texte_presse_papier;
	QDomDocument document_xml;
	if ((texte_presse_papier = QApplication::clipboard() -> text()) == QString()) return;
	if (!document_xml.setContent(texte_presse_papier)) return;
	scene -> fromXml(document_xml);
}

/**
	gere les clics et plus particulierement le clic du milieu (= coller pour X11)
*/
void SchemaView::mousePressEvent(QMouseEvent *e) {
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
Open a * .qet file in this SchemaView
@param filename Name of the file a open
@param error If the pointer is specified, this integer is set to 0 if the opening is successful,
1 if the queue does not exist,
2 if the file is not readable,
3 if the file is not an XML element,
4 if the opening of the file failed for another reason (that's not what's missing ^^)
@return true if the opening was successful, false otherwise
*/
bool SchemaView::open(QString n_fichier, int *erreur) {
	// verifie l'existence du file
	if (!QFileInfo(n_fichier).exists()) {
		if (erreur != NULL) *erreur = 1;
		return(false);
	}
	
	// ouvre le file
	QFile file(n_fichier);
	if (!file.open(QIODevice::ReadOnly)) {
		if (erreur != NULL) *erreur = 2;
		return(false);
	}
	
	// lit son contenu dans un QDomDocument
	QDomDocument document;
	if (!document.setContent(&file)) {
		if (erreur != NULL) *erreur = 3;
		file.close();
		return(false);
	}
	file.close();
	
	// construit le schema a partir du QDomDocument
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

void SchemaView::slot_selectionChanged() {
	emit(selectionChanged());
}

void SchemaView::closeEvent(QCloseEvent *event) {
	// demande d'abord a l'utilisateur s'il veut enregistrer le schema en cours
	QMessageBox::StandardButton reponse = QMessageBox::question(
		this,
		tr("Save the current schema?"),
		tr("Do you want to save the current schema ?"),
		QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel,
		QMessageBox::Cancel
	);
	bool retour;
	switch(reponse) {
		case QMessageBox::Cancel: retour = false;         break; // l'utilisateur annule : echec de la fermeture
		case QMessageBox::Yes:    retour = enregistrer(); break; // l'utilisateur dit oui : la reussite depend de l'enregistrement
		default:                  retour = true;                 // l'utilisateur dit non ou ferme le dialogue: c'est reussi
	}
	if (retour) event -> accept();
	else event -> ignore();
}

/**
	Methode enregistrant le schema dans le dernier nom de file connu.
	Si aucun nom de file n'est connu, cette methode appelle la methode enregistrer_sous
	@return true si l'enregistrement a reussi, false sinon
*/
bool SchemaView::enregistrer() {
	if (nom_fichier == QString()) return(enregistrer_sous());
	else return(private_enregistrer(nom_fichier));
}

/**

This method asks the user for a queue name to save the schema
If no name is entered, it returns false.
If the name does not end with the .qet extension, it is added.
If the save is successful, the name of the file is kept and the function returns true.
Otherwise, false is returned.
@return true if the registration was successful, false otherwise
@todo detect the desktop path automatically
*/
bool SchemaView::enregistrer_sous() {
	// demande un nom de file a l'utilisateur pour enregistrer le schema
	QString n_fichier = QFileDialog::getSaveFileName(
		this,
		tr("Enregistrer sous"),
		QDir::homePath(),
		tr("Schema QelectroTech (*.qet)")
	);
	// if no name is entered, return false.
	if (n_fichier == "") return(false);
	// si le nom ne se termine pas par l'extension .qet, celle-ci est ajoutee
	if (!n_fichier.endsWith(".qet", Qt::CaseInsensitive)) n_fichier += ".qet";
	// tente d'enregistrer le file
	bool resultat_enregistrement = private_enregistrer(n_fichier);
	// si l'enregistrement reussit, le nom du file est conserve
	if (resultat_enregistrement) {
		nom_fichier = n_fichier;
		setWindowTitle(nom_fichier + "[*]");
	}
	// retourne un booleen representatif de la reussite de l'enregistrement
	return(resultat_enregistrement);
}

/**
Private method managing the recording of the XML file. If it is not possible
to write to the file, this function displays an error message and returns false.
Otherwise, it returns true.
@param filename Name of the file in which the XML tree must be written
@return true if the registration was successful, false otherwise
*/
bool SchemaView::private_enregistrer(QString &n_fichier) {
	QFile file(n_fichier);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::warning(this, tr("Erreur"), tr("Impossible d'ecrire dans ce file"));
		return(false);
	}
	QTextStream out(&file);
	out.setCodec("UTF-8");
	out << scene -> toXml().toString(4);
	file.close();
	return(true);
}

