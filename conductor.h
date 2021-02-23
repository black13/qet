#ifndef CONDUCTEUR_H
	#define CONDUCTEUR_H
	#include <QtGui>
	#include "terminal.h"
	class Element;
	/**
		This class represents a driver. A conductor connects two element terminals.
	*/
	class Conductor : public QGraphicsPathItem {
		public:
		enum { Type = UserType + 1001 };
    	virtual int type() const { return Type; }
		Conductor(Terminal *, Terminal *, Element * = 0, QGraphicsScene * = 0);
		//virtual ~Conductor();
		
		void destroy();
		bool isDestroyed() const { return(destroyed); }
		void update(const QRectF & rect);
		void update(qreal x, qreal y, qreal width, qreal height);
		void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
		static bool valideXml(QDomElement &);
		
		/// First terminal to which the wire is attached
		Terminal *terminal1;
		/// Second terminal to which the wire is attached
		Terminal *terminal2;
		private:
		/// booleen indicating if the thread is still valid
		bool destroyed;
		
		void calculateConductor();
		bool surLeMemeAxe(Terminal::Orientation, Terminal::Orientation);
		bool estHorizontale(Terminal::Orientation a);
		bool estVerticale(Terminal::Orientation a);
	};
#endif
