#ifndef BORNE_H
	#define BORNE_H
	#define TAILLE_BORNE 4
	#include <QtGui>
	#include <QtXml>
	class Conducteur;
	class Element;
	class Schema;
	/**
	Class modeling the terminal of a device, that is to say a
	Possible connection for a driver.
	*/
	class Borne : public QGraphicsItem {
		public:
		// ENUM defining the orientation of the terminal
		enum Orientation {Nord, Sud, Est, Ouest};
		
		// Allows you to cast a quality qgraphicsitem with QGraphicsItem_cast
		enum { Type = UserType + 1002 };
    	virtual int type() const { return Type; }
		
		// constructors
		Borne();
		Borne(QPointF,      Borne::Orientation, Element * = 0, Schema * = 0);
		Borne(qreal, qreal, Borne::Orientation, Element * = 0, Schema * = 0);
		
		// destructors
		~Borne();
		
		// Implementation of pure virtual methods of QGraphicsItem
		void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
		QRectF boundingRect() const;
		
		// Methods of handling drivers related to this terminal
		bool addConducteur(Conducteur *);
		void removeConducteur(Conducteur *);
		inline int nbConducteurs() { return(liste_conducteurs.size()); }
		
		// read methods
		QList<Conducteur *> conducteurs() const; 
		Borne::Orientation orientation() const;
		inline QPointF amarrageConducteur() const { return(mapToScene(amarrage_conducteur)); }
		void updateConducteur();
		
		// Methods for Import / Export in XML format
		static bool valideXml(QDomElement  &);
		bool        fromXml  (QDomElement  &);
		QDomElement toXml    (QDomDocument &);
		
		// METHODS FOR MANAGEMENT OF EVENTS
		void hoverEnterEvent  (QGraphicsSceneHoverEvent *);
		void hoverMoveEvent   (QGraphicsSceneHoverEvent *);
		void hoverLeaveEvent  (QGraphicsSceneHoverEvent *);
		void mousePressEvent  (QGraphicsSceneMouseEvent *);
		void mouseMoveEvent   (QGraphicsSceneMouseEvent *);
		void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
		
		private:
		// pointer to the schema type QGraphicsscene (avoids some caste internally)
		Schema *schema_scene;
		// coordinates mooring points
		QPointF amarrage_conducteur;
		QPointF amarrage_elmt;
		// Orientation of the terminal
		Borne::Orientation sens;
		// list of conductors linked at this terminal
		QList<Conducteur *> liste_conducteurs;
		// pointer to a rectangle corresponding to the bounding rect;allows you to calculate the bounding rect only once;the pointer is because the compilo requires a constable method
		QRectF *br;
		Borne *borne_precedente;
		bool hovered;
		// Method initiating the different members of the terminal
		void initialise(QPointF, Borne::Orientation);
		// different colors that can be used for the "hover" effect
		QColor couleur_hovered;
		QColor couleur_neutre;
		QColor couleur_autorise;
		QColor couleur_prudence;
		QColor couleur_interdit;
	};
#endif
