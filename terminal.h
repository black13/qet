#ifndef BORNE_H
	#define BORNE_H
	#define TAILLE_BORNE 4
	#include <QtWidgets>
	#include <QtXml/QtXml>
	class Conductor;
	class Element;
	class Schema;
	/**
	Class modeling the "terminal" of a device, that is to say a
	connection possible for a Driver.
	*/
	class Terminal : public QGraphicsItem {
		public:
		// enum definissant l'orientation de la borne
		enum Orientation {Nord, Sud, Est, Ouest};
		
		// permet de caster un QGraphicsItem en Terminal avec qgraphicsitem_cast
		enum { Type = UserType + 1002 };
    	virtual int type() const { return Type; }
		
		// constructeurs
		Terminal();
		Terminal(QPointF,      Terminal::Orientation, Element * = 0, Schema * = 0);
		Terminal(qreal, qreal, Terminal::Orientation, Element * = 0, Schema * = 0);
		
		// destructeur
		~Terminal();
		
		// implementation des methodes virtuelles pures de QGraphicsItem
		void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
		QRectF boundingRect() const;
		
		// methodes de manipulation des conducteurs lies a cette borne
		bool addConducteur(Conductor *);
		void removeConducteur(Conductor *);
		inline int nbConducteurs() { return(liste_conducteurs.size()); }
		
		// methodes de lecture
		QList<Conductor *> conducteurs() const; 
		Terminal::Orientation orientation() const;
		inline QPointF amarrageConducteur() const { return(mapToScene(amarrage_conducteur)); }
		void updateConducteur();
		
		// methodes relatives a l'import/export au format XML
		static bool valideXml(QDomElement  &);
		bool        fromXml  (QDomElement  &);
		QDomElement toXml    (QDomDocument &);
		
		// methodes de gestion des evenements
		void hoverEnterEvent  (QGraphicsSceneHoverEvent *);
		void hoverMoveEvent   (QGraphicsSceneHoverEvent *);
		void hoverLeaveEvent  (QGraphicsSceneHoverEvent *);
		void mousePressEvent  (QGraphicsSceneMouseEvent *);
		void mouseMoveEvent   (QGraphicsSceneMouseEvent *);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
		
		private:
		// pointeur vers la QGraphicsScene de type Schema (evite quelques casts en interne)
		Schema *schema_scene;
		// coordonnees des points d'amarrage
		QPointF amarrage_conducteur;
		QPointF amarrage_elmt;
		// orientation de la borne
		Terminal::Orientation sens;
		// liste des conducteurs lies a cette borne
		QList<Conductor *> liste_conducteurs;
		// pointeur vers un rectangle correspondant au bounding rect ; permet de ne calculer le bounding rect qu'une seule fois ; le pointeur c'est parce que le compilo exige une methode const
		QRectF *br;
		Terminal *borne_precedente;
		bool hovered;
		// methode initialisant les differents membres de la borne
		void initialise(QPointF, Terminal::Orientation);
		// differentes couleurs utilisables pour l'effet "hover"
		QColor couleur_hovered;
		QColor couleur_neutre;
		QColor couleur_autorise;
		QColor couleur_prudence;
		QColor couleur_interdit;
	};
#endif
