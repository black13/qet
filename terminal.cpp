#include "terminal.h"
#include "schema.h"
#include "element.h"
#include "conductor.h"
#include "debug.h"
/**
	Fonction privee pour initialiser la borne.
	@param pf  position du point d'amarrage pour un conducteur
	@param o   orientation de la borne : Qt::Horizontal ou Qt::Vertical
*/
void Terminal::initialise(QPointF pf, Terminal::Orientation o) {
	// definition du pount d'amarrage pour un conducteur
	amarrage_conducteur  = pf;
	
	// definition de l'orientation de la borne (par defaut : sud)
	if (o < Terminal::Nord || o > Terminal::Ouest) sens = Terminal::Sud;
	else sens = o;
	
	// calcul de la position du point d'amarrage a l'element
	amarrage_elmt = amarrage_conducteur;
	switch(sens) {
		case Terminal::Nord  : amarrage_elmt += QPointF(0, TAILLE_BORNE);  break;
		case Terminal::Est   : amarrage_elmt += QPointF(-TAILLE_BORNE, 0); break;
		case Terminal::Ouest : amarrage_elmt += QPointF(TAILLE_BORNE, 0);  break;
		case Terminal::Sud   :
		default           : amarrage_elmt += QPointF(0, -TAILLE_BORNE);
	}
	
	// par defaut : pas de conducteur
	
	// QRectF null
	br = new QRectF();
	borne_precedente = NULL;
	// divers
	//setAcceptsHoverEvents(true);
	setAcceptHoverEvents(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	hovered = false;
	setToolTip("Terminal");
	couleur_neutre   = QColor(Qt::blue);
	couleur_autorise = QColor(Qt::darkGreen);
	couleur_prudence = QColor("#ff8000");
	couleur_interdit = QColor(Qt::red);
	couleur_hovered  = couleur_neutre;
}

/**
	Constructeur par defaut
*/
Terminal::Terminal() : QGraphicsItem() {
	initialise(QPointF(0.0, 0.0), Terminal::Sud);
	schema_scene = 0;
}

/**
	initialise une terminal
	@param pf  position du point d'amarrage pour un conductor
	@param o   orientation de la terminal : Qt::Horizontal ou Qt::Vertical
	@param e   Element auquel cette terminal appartient
	@param s   Scene sur laquelle figure cette terminal
*/
Terminal::Terminal(QPointF pf, Terminal::Orientation o, Element *e, Schema *s) : QGraphicsItem(e) {
	initialise(pf, o);
	schema_scene = s;
}

/**

initializes a terminal
@param pf_x Abscissa of the mooring point for a conductor
@param pf_y Order of the mooring point for a driver
@param o terminal orientation: Qt :: Horizontal or Qt :: Vertical
@param e Element to which this terminal belongs
@param s Scene on which this terminal appears
*/
Terminal::Terminal(qreal pf_x, qreal pf_y, Terminal::Orientation o, Element *e, Schema *s) : QGraphicsItem(e) {
	initialise(QPointF(pf_x, pf_y), o);
}

/**
	Destructeur
*/
Terminal::~Terminal() {
	delete br;
}

/**
	Permet de connaitre l'orientation de la borne. Si le parent de la borne
	est bien un Element, cette fonction renvoie l'orientation par rapport a
	la scene de la borne, en tenant compte du fait que l'element ait pu etre
	pivote. Sinon elle renvoie son sens normal.
	@return L'orientation actuelle de la Terminal.
*/
Terminal::Orientation Terminal::orientation() const {
	//true pour une orientation verticale, false pour une orientation horizontale
	if (Element *elt = qgraphicsitem_cast<Element *>(parentItem())) {
		if (elt -> orientation()) return(sens);
		else {
			Terminal::Orientation retour;
			switch(sens) {
				case Terminal::Nord  : retour = Terminal::Ouest; break;
				case Terminal::Est   : retour = Terminal::Nord;  break;
				case Terminal::Ouest : retour = Terminal::Sud;   break;
				case Terminal::Sud   :
				default           : retour = Terminal::Est;
			}
			return(retour);
		}
	} else return(sens);
}

/**
Assign a conductor to the terminal
@param f The conductor to be attached to this terminal
*/
bool Terminal::addConducteur(Conductor *f) {
	trace_msg("");
	// pointeur 0 refuse
	if (!f) return(false);
	
	// une seule des deux bornes du conducteur doit etre this
	Q_ASSERT_X((f -> borne1 == this ^ f -> borne2 == this), "Terminal::addConducteur", "Le conducteur devrait etre relie exactement une fois a la borne en cours");
	
	// determine l'autre borne a laquelle cette borne va etre relie grace au conducteur
	Terminal *autre_borne = (f -> borne1 == this) ? f -> borne2 : f -> borne1;
	
	// verifie que la borne n'est pas deja reliee avec l'autre borne
	bool deja_liees = false;
	foreach (Conductor* conducteur, liste_conducteurs) {
		if (conducteur -> borne1 == autre_borne || conducteur -> borne2 == autre_borne) deja_liees = true;
	}
	
	// si les deux bornes sont deja reliees, on refuse d'ajouter le conducteur
	if (deja_liees) return(false);
	
	// sinon on ajoute le conducteur
	liste_conducteurs.append(f);
	return(true);
}

void Terminal::removeConducteur(Conductor *f) {
	int index = liste_conducteurs.indexOf(f);
	if (index == -1) return;
	liste_conducteurs.removeAt(index);
}

/**
	Fonction de dessin des bornes
	@param p Le QPainter a utiliser
	@param options Les options de dessin
	@param widget Le widget sur lequel on dessine
*/
void Terminal::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) {
	p -> save();
	trace_msg("");
	//annulation des renderhints
	p -> setRenderHint(QPainter::Antialiasing,          false);
	p -> setRenderHint(QPainter::TextAntialiasing,      false);
	p -> setRenderHint(QPainter::SmoothPixmapTransform, false);
	
	// on travaille avec les coordonnees de l'element parent
	QPointF f = mapFromParent(amarrage_conducteur);
	QPointF e = mapFromParent(amarrage_elmt);
	
	QPen t;
	t.setWidthF(1.0);
	
	// dessin de la borne en rouge
	t.setColor(Qt::red);
	p -> setPen(t);
	p -> drawLine(f, e);
	
	// dessin du point d'amarrage au conducteur en bleu
	t.setColor(couleur_hovered);
	p -> setPen(t);
	p -> setBrush(couleur_hovered);
	if (hovered) p -> drawEllipse(((int)f.x())-2, ((int)f.y())-2, 5, 5);
	else p -> drawPoint(f);
	
	p -> restore();
}

/**
	@return Le rectangle (en precision flottante) delimitant la borne et ses alentours.
*/
QRectF Terminal::boundingRect() const {

	trace_msg("");
	if (br -> isNull()) {
		qreal afx = amarrage_conducteur.x();
		qreal afy = amarrage_conducteur.y();
		qreal aex = amarrage_elmt.x();
		qreal aey = amarrage_elmt.y();
		QPointF origine;
		origine = (afx <= aex && afy <= aey ? amarrage_conducteur : amarrage_elmt);
		origine += QPointF(-3.0, -3.0);
		qreal w = qAbs((int)(afx - aex)) + 7;
		qreal h = qAbs((int)(afy - aey)) + 7;
		*br = QRectF(origine, QSizeF(w, h));
	}
	return(*br);
}

/**
	Gere l'entree de la souris sur la zone de la Terminal.
*/
void Terminal::hoverEnterEvent(QGraphicsSceneHoverEvent *) {
	hovered = true;
	update();
}

/**
	Gere les mouvements de la souris sur la zone de la Terminal.
*/
void Terminal::hoverMoveEvent(QGraphicsSceneHoverEvent *) {
}

/**
	Gere le fait que la souris sorte de la zone de la Terminal.
*/
void Terminal::hoverLeaveEvent(QGraphicsSceneHoverEvent *) {
	hovered = false;
	update();
}

/**
	Gere le fait qu'on enfonce un bouton de la souris sur la Terminal.
	@param e L'evenement souris correspondant
*/
void Terminal::mousePressEvent(QGraphicsSceneMouseEvent *e) {
	if (Schema *s = qobject_cast<Schema *>(scene())) {
		s -> setDepart(mapToScene(QPointF(amarrage_conducteur)));
		s -> setArrivee(e -> scenePos());
		s -> poseConducteur(true);
		setCursor(Qt::CrossCursor);
	}
	//QGraphicsItem::mouseReleaseEvent(e);
}

/**
	Gere le fait qu'on bouge la souris sur la Terminal.
	@param e L'evenement souris correspondant
*/
void Terminal::mouseMoveEvent(QGraphicsSceneMouseEvent *e) {
	// pendant la pose d'un conducteur, on adopte un autre curseur 
	setCursor(Qt::CrossCursor);
	
	// d'un mouvement a l'autre, il faut retirer l'effet hover de la borne precedente
	if (borne_precedente != NULL) {
		if (borne_precedente == this) hovered = true;
		else borne_precedente -> hovered = false;
		borne_precedente -> couleur_hovered = borne_precedente -> couleur_neutre;
		borne_precedente -> update();
	}
	
	// si la scene est un Schema, on actualise le poseur de conducteur
	if (Schema *s = qobject_cast<Schema *>(scene())) s -> setArrivee(e -> scenePos());
	
	// on recupere la liste des qgi sous le pointeur
	QList<QGraphicsItem *> qgis = scene() -> items(e -> scenePos());
	
	/* le qgi le plus haut
	   = le poseur de conducteur
	   = le premier element de la liste
	   = la liste ne peut etre vide
	   = on prend le deuxieme element de la liste
	*/
	Q_ASSERT_X(!(qgis.isEmpty()), "Terminal::mouseMoveEvent", "La liste d'items ne devrait pas etre vide");
	
	// s'il y a autre chose que le poseur de conducteur dans la liste
	if (qgis.size() > 1) {
		// on prend le deuxieme element de la liste
		QGraphicsItem *qgi = qgis.at(1);
		// si le qgi est une borne...
		if (Terminal *p = qgraphicsitem_cast<Terminal *>(qgi)) {
			// ...on lui applique l'effet hover approprie
			if (p == this) {
				// effet si l'on hover sur la borne de depart
				couleur_hovered = couleur_interdit;
			} else if (p -> parentItem() == parentItem()) {
				// effet si l'on hover sur une borne du meme appareil
				if (((Element *)parentItem()) -> connexionsInternesAcceptees())
					p -> couleur_hovered = p -> couleur_autorise;
				else p -> couleur_hovered = p -> couleur_interdit;
			} else if (p -> nbConducteurs()) {
				// si la borne a deja un conducteur
				// verifie que cette borne n'est pas deja reliee a l'autre borne
				bool deja_reliee = false;
				foreach (Conductor *f, liste_conducteurs) {
					if (f -> borne1 == p || f -> borne2 == p) {
						deja_reliee = true;
						break;
					}
				}
				// interdit si les bornes sont deja reliees, prudence sinon
				p -> couleur_hovered = deja_reliee ? p -> couleur_interdit : p -> couleur_prudence;
			} else {
				// effet si on peut poser le conducteur
				p -> couleur_hovered = p -> couleur_autorise;
			}
			borne_precedente = p;
			p -> hovered = true;
			p -> update();
		}
	}
}

/**
	Gere le fait qu'on relache la souris sur la Terminal.
	@param e L'evenement souris correspondant
*/
void Terminal::mouseReleaseEvent(QGraphicsSceneMouseEvent *e) {
	setCursor(Qt::ArrowCursor);
	borne_precedente = NULL;
	couleur_hovered  = couleur_neutre;
	// verifie que la scene est bien un Schema
	if (Schema *s = qobject_cast<Schema *>(scene())) {
		// on arrete de dessiner l'apercu du conducteur
		s -> poseConducteur(false);
		// on recupere l'element sous le pointeur lors du MouseReleaseEvent
		QGraphicsItem *qgi = s -> itemAt(e -> scenePos(),QTransform() );
		// s'il n'y a rien, on arrete la
		if (!qgi) return;
		// idem si l'element obtenu n'est pas une borne
		Terminal *p = qgraphicsitem_cast<Terminal *>(qgi);
		if (!p) return;
		// on remet la couleur de hover a sa valeur par defaut
		p -> couleur_hovered = p -> couleur_neutre;
		// idem s'il s'agit de la borne actuelle
		if (p == this) return;
		// idem s'il s'agit d'une borne de l'element actuel et que l'element n'a pas le droit de relier ses propres bornes
		bool cia = ((Element *)parentItem()) -> connexionsInternesAcceptees();
		if (!cia) foreach(QGraphicsItem *item, parentItem() -> childItems()) if (item == p) return;
		// derniere verification : verifier que cette terminal n'est pas deja reliee a l'autre terminal
		foreach (Conductor *f, liste_conducteurs) if (f -> borne1 == p || f -> borne2 == p) return;
		// autrement, on pose un conducteur
		new Conductor(this, (Terminal *)qgi, 0, scene());
	}
}

/**
	Met a jour l'eventuel conducteur relie a la Terminal.
*/
void Terminal::updateConducteur() {
	if (scene()) {
		foreach (Conductor *conductor, liste_conducteurs) if (!conductor -> isDestroyed()) conductor -> update(QRectF()/*scene()->sceneRect()*/);
	}
}

/**
	@return La liste des conducteurs lies a cette borne
*/
QList<Conductor *> Terminal::conducteurs() const {
	return(liste_conducteurs);
}

/**
	Methode d'export en XML
	@param doc Le Document XML a utiliser pour creer l'element XML
	@return un QDomElement representant cette borne
*/
QDomElement Terminal::toXml(QDomDocument &doc) {
	QDomElement qdo = doc.createElement("borne");
	qdo.setAttribute("x", amarrage_elmt.x());
	qdo.setAttribute("y", amarrage_elmt.y());
	qdo.setAttribute("orientation", sens);
	return(qdo);
}

/**
	Permet de savoir si un element XML represente une borne
	@param e Le QDomElement a analyser
	@return true si le QDomElement passe en parametre est une borne, false sinon
*/
bool Terminal::valideXml(QDomElement &borne) {
	// verifie le nom du tag
	if (borne.tagName() != "borne") return(false);
	
	// verifie la presence des attributs minimaux
	if (!borne.hasAttribute("x")) return(false);
	if (!borne.hasAttribute("y")) return(false);
	if (!borne.hasAttribute("orientation")) return(false);
	
	bool conv_ok;
	// parse l'abscisse
	borne.attribute("x").toDouble(&conv_ok);
	if (!conv_ok) return(false);
	
	// parse l'ordonnee
	borne.attribute("y").toDouble(&conv_ok);
	if (!conv_ok) return(false);
	
	// parse l'id
	borne.attribute("id").toInt(&conv_ok);
	if (!conv_ok) return(false);
	
	// parse l'orientation
	int borne_or = borne.attribute("orientation").toInt(&conv_ok);
	if (!conv_ok) return(false);
	if (borne_or != Terminal::Nord && borne_or != Terminal::Sud && borne_or != Terminal::Est && borne_or != Terminal::Ouest) return(false);
	
	// a ce stade, la borne est syntaxiquement correcte
	return(true);
}

/**
	Permet de savoir si un element XML represente cette borne. Attention, l'element XML n'est pas verifie
	@param e Le QDomElement a analyser
	@return true si la borne "se reconnait" (memes coordonnes, meme orientation), false sinon
*/
bool Terminal::fromXml(QDomElement &borne) {
	return (
		borne.attribute("x").toDouble() == amarrage_elmt.x() &&\
		borne.attribute("y").toDouble() == amarrage_elmt.y() &&\
		borne.attribute("orientation").toInt() == sens
	);
}
