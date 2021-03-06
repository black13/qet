#include "entree.h"

/**
	Constructeur
	@param parent Le QObject parent de l'element.
	@param scene  La scene sur laquelle l'element est affiche
*/
Entree::Entree(QGraphicsItem *parent, Schema *scene) : FixedElement(parent, scene) {
	// taille et hotspot
	setSize(20, 40);
	setHotspot(QPoint(10, 15));
	
	// ajout d'une borne a l'element
	new Terminal(0,  15, Terminal::Sud, this, scene);
}

/**
	@return Le nombre actuel de bornes de l'element
*/
int Entree::nbBornes() const {
	return(1);
}

/**
	Fonction qui effectue le rendu graphique du contacteur
	@param p Le QPainter a utiliser pour dessiner l'element
	@param o Les options de dessin
*/
void Entree::paint(QPainter *p, const QStyleOptionGraphicsItem *) {
	//int ret = _CrtDbgReportW(_CRT_ERROR, _CRT_WIDE(__FILE__), __LINE__, _CRT_WIDE(QT_VERSION_STR), reinterpret_cast<const wchar_t *> (QString::fromLatin1("video/").utf16()));
	// traits de couleur noire
	QPen t;
	t.setColor(Qt::black);
	t.setWidthF(1.0);
	t.setJoinStyle(Qt::MiterJoin);
	p -> setPen(t);
	p -> setBrush(Qt::black);
	
	// Dessin du triangle
	static const QPointF points[3] = {
		QPointF(-7.5, -13),
		QPointF( 7.5, -13),
		QPointF( 0.0, 0.0)
	};
	
	p -> drawPolygon(points, 3);
	p -> setBrush(Qt::NoBrush);
	
	// une ligne JAMAIS antialiasee (annulation des renderhints)
	p -> save();
	p -> setRenderHint(QPainter::Antialiasing,          false);
	p -> setRenderHint(QPainter::TextAntialiasing,      false);
	p -> setRenderHint(QPainter::SmoothPixmapTransform, false);
	p -> drawLine(0, 0, 0, 13);
	p -> restore();
}

/**
	@return l'ID du type "Contactor"
*/
QString Entree::typeId() {
	return(QString("2"));
}
