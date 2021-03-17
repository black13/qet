#ifndef PANELAPPAREILS_H
	#define PANELAPPAREILS_H
	#include <QtGui>
	/**
	This class represents the panel of devices (as an element
	graph) in which the user chooses the components of
	His choice and deposits them on the schema by Drag'n Drop.
	*/
	class PanelAppareils : public QListWidget {
		Q_OBJECT
		public:
		PanelAppareils(QWidget * = 0);
		public slots:
		void dragMoveEvent(QDragMoveEvent *);
		void dropEvent(QDropEvent *);
		void startDrag(Qt::DropActions);
	};
#endif
