#include <math.h>
#include "conducteur.h"
#include "contacteur.h"
#include "elementperso.h"
#include "schema.h"

/**
	Constructor
	@param parent the parent QObject of the schema
*/
Schema::Schema(QObject *parent) : QGraphicsScene(parent) {
	setBackgroundBrush(Qt::white);
	poseur_de_conducteur = new QGraphicsLineItem(0, 0);
	poseur_de_conducteur -> setZValue(1000000);
	QPen t;
	t.setColor(Qt::black);
	t.setWidthF(1.5);
	t.setStyle(Qt::DashLine);
	poseur_de_conducteur -> setPen(t);
	poseur_de_conducteur -> setLine(QLineF(QPointF(0.0, 0.0), QPointF(0.0, 0.0)));
	doit_dessiner_grille = true;
	connect(this, SIGNAL(changed(const QList<QRectF> &)), this, SLOT(slot_checkSelectionChange()));
}

/**
	Draw the scraps from the schema, CAD the grid.
	@param p the qpainter to use to draw
	@param r the rectangle of the area to draw
*/
void Schema::drawBackground(QPainter *p, const QRectF &r) {
	p -> save();
	
	// Deactive all antialiasing
	p -> setRenderHint(QPainter::Antialiasing, false);
	p -> setRenderHint(QPainter::TextAntialiasing, false);
	p -> setRenderHint(QPainter::SmoothPixmapTransform, false);
	
	// draw a white background
	p -> setPen(Qt::NoPen);
	p -> setBrush(Qt::white);
	p -> drawRect(r);
	
	if (doit_dessiner_grille) {
		// draw the points of the grid
		p -> setPen(Qt::black);
		p -> setBrush(Qt::NoBrush);
		qreal limite_x = r.x() + r.width();
		qreal limite_y = r.y() + r.height();
		
		int g_x = (int)ceil(r.x());
		while (g_x % GRILLE_X) ++ g_x;
		int g_y = (int)ceil(r.y());
		while (g_y % GRILLE_Y) ++ g_y;
		
		for (int gx = g_x ; gx < limite_x ; gx += GRILLE_X) {
			for (int gy = g_y ; gy < limite_y ; gy += GRILLE_Y) {
				p -> drawPoint(gx, gy);
			}
		}
		
		p -> drawLine(0, 0, 0, 10);
		p -> drawLine(0, 0, 10, 0);
	}
	p -> restore();
}

QImage Schema::toImage() {
	
	QRectF vue = itemsBoundingRect();
	// margin = 5% of the length needed
	qreal marge = 0.05 * vue.width();
	vue.translate(-marge, -marge);
	vue.setWidth(vue.width() + 2.0 * marge);
	vue.setHeight(vue.height() + 2.0 * marge);
	QSize dimensions_image = vue.size().toSize();
	
	QImage pix = QImage(dimensions_image, QImage::Format_RGB32);
	QPainter p;
	bool painter_ok = p.begin(&pix);
	if (!painter_ok) return(QImage());
	
	// rendering antialiasis
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setRenderHint(QPainter::TextAntialiasing, true);
	p.setRenderHint(QPainter::SmoothPixmapTransform, true);
	
	render(&p, pix.rect(), vue, Qt::KeepAspectRatio);
	p.end();
	return(pix);
}

/**
	Exports all or part of the schema
	@param schema booleen (A true default) indicating whether the XML Genere must represent the entire schema or only the selected elements
	@return Un Document XML (QDomDocument)
*/
QDomDocument Schema::toXml(bool schema) {
	// document
	QDomDocument document;
	
	// root of the XML tree
	QDomElement racine = document.createElement("schema");
	
	// SCHEMA PROPERTIES
	if (schema) {
		if (!auteur.isNull()) racine.setAttribute("auteur", auteur);
		if (!date.isNull())   racine.setAttribute("date", date.toString("yyyyMMdd"));
		if (!titre.isNull())  racine.setAttribute("titre", titre);
	}
	document.appendChild(racine);
	
	// if the schema does not contain any element (and therefore no conductors), the XML document is returned
	if (items().isEmpty()) return(document);
	
	// Creation of two lists: one that contains the elements, one that contains the drivers
	QList<Element *> liste_elements;
	QList<Conducteur *> liste_conducteurs;
	
	// Determine les elements a � XMLiser �
	foreach(QGraphicsItem *qgi, items()) {
		if (Element *elmt = qgraphicsitem_cast<Element *>(qgi)) {
			if (schema) liste_elements << elmt;
			else if (elmt -> isSelected()) liste_elements << elmt;
		} else if (Conducteur *f = qgraphicsitem_cast<Conducteur *>(qgi)) {
			if (schema) liste_conducteurs << f;
			// when not exporting all the schema, the nonselected conductors must be removed
			// and for now, non-selected drivers are drivers including one of the elements is not connected
			else if (f -> borne1 -> parentItem() -> isSelected() && f -> borne2 -> parentItem() -> isSelected()) liste_conducteurs << f;
		}
	}
	
	// Registration of elements
	if (liste_elements.isEmpty()) return(document);
	int id_borne = 0;
	// correspondence table between the addresses of the terminals and their ids
	QHash<Borne *, int> table_adr_id;
	QDomElement elements = document.createElement("elements");
	foreach(Element *elmt, liste_elements) {
		QDomElement element = document.createElement("element");
		
		// type, position, selection et orientation
		element.setAttribute("type", QFileInfo(elmt -> typeId()).fileName());
		element.setAttribute("x", elmt -> pos().x());
		element.setAttribute("y", elmt -> pos().y());
		if (elmt -> isSelected()) element.setAttribute("selected", "selected");
		element.setAttribute("sens", elmt -> orientation() ? "true" : "false");
		
		// records of the terminals of each device
		QDomElement bornes = document.createElement("bornes");
		// For each child of the element
		foreach(QGraphicsItem *child, elmt -> children()) {
			// if this child is a terminal
			if (Borne *p = qgraphicsitem_cast<Borne *>(child)) {
			// then we record the terminal
				QDomElement borne = p -> toXml(document);
				borne.setAttribute("id", id_borne);
				table_adr_id.insert(p, id_borne ++);
				bornes.appendChild(borne);
			}
		}
		element.appendChild(bornes);
		
		/**
			@todo call a virtual method of the element class that will allow
			to developers of elements to customize the registration of the elements
		*/
		elements.appendChild(element);
	}
	racine.appendChild(elements);
	
	// recording drivers
	if (liste_conducteurs.isEmpty()) return(document);
	QDomElement conducteurs = document.createElement("conducteurs");
	foreach(Conducteur *f, liste_conducteurs) {
		QDomElement conducteur = document.createElement("conducteur");
		conducteur.setAttribute("borne1", table_adr_id.value(f -> borne1));
		conducteur.setAttribute("borne2", table_adr_id.value(f -> borne2));
		conducteurs.appendChild(conducteur);
	}
	racine.appendChild(conducteurs);
	
	// on retourne le document XML ainsi genere
	return(document);
}

void Schema::reset() {
	/// @todo Implement this function
}

/**
	Imports the schema described in an XML document.If a position is preceded, the imports are positioned in order to ensure that the lower left corner of the smallest rectangle that can all around them (the bounding rect) is at this position.
	@param document the XML document to analyze
	@Param position the position of the schema imports
	@return true if the import has managed, false otherwise
*/
bool Schema::fromXml(QDomDocument &document, QPointF position) {
	QDomElement racine = document.documentElement();
	// the first element must be a schema
	if (racine.tagName() != "schema") return(false);
	// Reading the attributes of this scheme
	auteur = racine.attribute("auteur");
	titre  = racine.attribute("titre");
	date   = QDate::fromString(racine.attribute("date"), "yyyyMMdd");
	
	// If the root does not have a child: the loading is finished (empty schema)
	if (racine.firstChild().isNull()) return(true);
	
	// Loading all the elements of the XML file
	QList<Element *> elements_ajoutes;
	//uint nb_elements = 0;
	QHash< int, Borne *> table_adr_id;
	QHash< int, Borne *> &ref_table_adr_id = table_adr_id;
	for (QDomNode node = racine.firstChild() ; !node.isNull() ; node = node.nextSibling()) {
		// We are interested in the element XML "Elements" (= group of elements)
		QDomElement elmts = node.toElement();
		if(elmts.isNull() || elmts.tagName() != "elements") continue;
		// Children's course of the XML element "Elements"
		for (QDomNode n = elmts.firstChild() ; !n.isNull() ; n = n.nextSibling()) {
			// We are interested in the element XML "ELEMENT" (Elements themselves)
			QDomElement e = n.toElement();
			if (e.isNull() || !Element::valideXml(e)) continue;
			Element *element_ajoute;
			if ((element_ajoute = elementFromXml(e, ref_table_adr_id)) != NULL) elements_ajoutes << element_ajoute;
			else qDebug("Le chargement d'un element a echoue");
		}
	}
	
	// No element has been added - no need to look for drivers - loading is finished
	if (!elements_ajoutes.size()) return(true);
	
	// Gene the translation of new elements if it is requested
	if (position != QPointF()) {
		// Determine What is the left area left of the rectangle surrounding the elements adds
		qreal minimum_x = 0, minimum_y = 0;
		bool init = false;
		foreach (Element *elmt_ajoute, elements_ajoutes) {
			QPointF csg = elmt_ajoute -> mapToScene(elmt_ajoute -> boundingRect().topLeft());
			qreal px = csg.x();
			qreal py = csg.y();
			if (!init) {
				minimum_x = px;
				minimum_y = py;
				init = true;
			} else {
				if (px < minimum_x) minimum_x = px;
				if (py < minimum_y) minimum_y = py;
			}
		}
		qreal diff_x = position.x() - minimum_x;
		qreal diff_y = position.y() - minimum_y;
		foreach (Element *elmt_ajoute, elements_ajoutes) {
			elmt_ajoute -> setPos(elmt_ajoute -> pos().x() + diff_x, elmt_ajoute -> pos().y() + diff_y);
		}
	}
	
	// Load all the conductors of the XML file
	for (QDomNode node = racine.firstChild() ; !node.isNull() ; node = node.nextSibling()) {
		// We are interested in the XML element "conductors" (= group of conductors)
		QDomElement conducteurs = node.toElement();
		if(conducteurs.isNull() || conducteurs.tagName() != "conducteurs") continue;
		// COURSES OF CHILDREN OF THE XML ELEMENT "Conductors"
		for (QDomNode n = conducteurs.firstChild() ; !n.isNull() ; n = n.nextSibling()) {
			// We are interested in the element XML "ELEMENT" (Elements themselves)
			QDomElement f = n.toElement();
			if (f.isNull() || !Conducteur::valideXml(f)) continue;
			// verify that the terminals that the driver connects are known
			int id_p1 = f.attribute("borne1").toInt();
			int id_p2 = f.attribute("borne2").toInt();
			if (table_adr_id.contains(id_p1) && table_adr_id.contains(id_p2)) {
				// Install the driver ... if it is possible
				Borne *p1 = table_adr_id.value(id_p1);
				Borne *p2 = table_adr_id.value(id_p2);
				if (p1 != p2) {
					bool peut_poser_conducteur = true;
					bool cia = ((Element *)p2 -> parentItem()) -> connexionsInternesAcceptees();
					if (!cia) foreach(QGraphicsItem *item, p2 -> parentItem() -> children()) if (item == p1) peut_poser_conducteur = false;
					if (peut_poser_conducteur) new Conducteur(table_adr_id.value(id_p1), table_adr_id.value(id_p2), 0, this);
				}
			} else qDebug() << "Le chargement du conducteur" << id_p1 << id_p2 << "a echoue";
		}
	}
	return(true);
}

/**
	Add to the schema the element corresponding to the omelification passes in parametre
	@param e QDomElement a analyser
	@param table_id_adr correspondence table between integers and terminals
	@return true if the addition perfectly successful, false otherwise
*/
Element *Schema::elementFromXml(QDomElement &e, QHash<int, Borne *> &table_id_adr) {
	// Creates an element whose type corresponds to the type ID
	QString type = e.attribute("type");
	int etat;
	Element *nvel_elmt = new ElementPerso(type, 0, 0, &etat);
	/*switch(e.attribute("type").toInt()) {
		case 0: nvel_elmt = new Contacteur(); break;
		case 1: nvel_elmt = new DEL();        break;
		case 2: nvel_elmt = new Entree();     break;
	}*/
	if (etat != 0) return(false);
	bool retour = nvel_elmt -> fromXml(e, table_id_adr);
	if (!retour) {
		delete nvel_elmt;
	} else {
		// ajout de l'element au schema
		addItem(nvel_elmt);
		nvel_elmt -> setPos(e.attribute("x").toDouble(), e.attribute("y").toDouble());
		nvel_elmt -> setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
		if (e.attribute("sens") == "false") nvel_elmt -> invertOrientation();
		nvel_elmt -> setSelected(e.attribute("selected") == "selected");
	}
	return(retour ? nvel_elmt : NULL);
}

void Schema::slot_checkSelectionChange() {
	static QList<QGraphicsItem *> cache_selecteditems = QList<QGraphicsItem *>();
	QList<QGraphicsItem *> selecteditems = selectedItems();
	if (cache_selecteditems != selecteditems) emit(selectionChanged());
	cache_selecteditems = selecteditems;
}
