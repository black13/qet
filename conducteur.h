#ifndef CONDUCTEUR_H
	#define CONDUCTEUR_H
	#include <QtGui>
	#include "borne.h"
	class Element;
	/**
		This class represents a driver.A driver connects two element terminals.
	*/
	class Conducteur : public QGraphicsPathItem {
		public:
		enum { Type = UserType + 1001 };
    	virtual int type() const { return Type; }
		Conducteur(Borne *, Borne *, Element * = 0, QGraphicsScene * = 0);
		// virtual ~ conductor ();
		
		void destroy();
		bool isDestroyed() const { return(destroyed); }
		void update(const QRectF & rect);
		void update(qreal x, qreal y, qreal width, qreal height);
		void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
		static bool valideXml(QDomElement &);
		
		/// first terminal at which the wire is attached
		Borne *borne1;
		/// second terminal at which the thread is attached
		Borne *borne2;
		private:
		/// boolen indicating if the wire is still valid
		bool destroyed;
		
		void calculeConducteur();
		bool surLeMemeAxe(Borne::Orientation, Borne::Orientation);
		bool estHorizontale(Borne::Orientation a);
		bool estVerticale(Borne::Orientation a);
	};
#endif
