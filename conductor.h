#ifndef CONDUCTEUR_H
	#define CONDUCTEUR_H
	#include <QtGui>
	#include "terminal.h"
	class Element;
	/**
		Cette classe represente un conducteur. Un conducteur relie deux bornes d'element.
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
		
		///Premiere borne a laquelle le fil est rattache
		Terminal *borne1;
		///Deuxieme borne a laquelle le fil est rattache
		Terminal *borne2;
		private:
		/// booleen indiquant si le fil est encore valide
		bool destroyed;
		
		void calculeConducteur();
		bool surLeMemeAxe(Terminal::Orientation, Terminal::Orientation);
		bool estHorizontale(Terminal::Orientation a);
		bool estVerticale(Terminal::Orientation a);
	};
#endif
