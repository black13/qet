#ifndef QETAPP_H
	#define QETAPP_H
	#include <QtGui>
	class SchemaVue;
	class PanelAppareils;
	/**
	This class represents the main window of QELECTROTECH and,
	IPSO FACTO, most of the QELECTROTECH graphical interface.
	It is a QMainWindow object with an object � schema � as a central widget
	and an � panel of appliances � as a widget � dock �.
	*/
	class QETApp : public QMainWindow {
		Q_OBJECT
		public:
		QETApp(QWidget *parent=0);
		void closeEvent(QCloseEvent * event );
		void addSchemaVue(SchemaVue *);
		
		public slots:
		void systray(QSystemTrayIcon::ActivationReason raison);
		void systrayReduire();
		void systrayRestaurer();
		void quitter();
		void toggleFullScreen();
		void toggleAntialiasing();
		void aPropos();
		void dialogue_imprimer();
		void dialogue_exporter();
		bool dialogue_enregistrer_sous();
		bool enregistrer();
		bool nouveau();
		bool ouvrir();
		bool fermer();
		
		protected:
		// Feasible actions through menus in the QELECTROTECH application
		QAction *mode_selection;
		QAction *mode_visualise;
		QAction *nouveau_fichier;
		QAction *ouvrir_fichier;
		QAction *fermer_fichier;
		QAction *enr_fichier;
		QAction *enr_fichier_sous;
		QAction *importer;
		QAction *exporter;
		QAction *imprimer;
		QAction *quitter_qet;
		QAction *annuler;
		QAction *refaire;
		QAction *couper;
		QAction *copier;
		QAction *coller;
		QAction *sel_tout;
		QAction *sel_rien;
		QAction *sel_inverse;
		QAction *supprimer;
		QAction *selectionner;
		QAction *pivoter;
		QAction *poser_fil;
		QAction *masquer_appli;
		QAction *restaurer_appli;
		QAction *zoom_avant;
		QAction *zoom_arriere;
		QAction *zoom_adapte;
		QAction *zoom_reset;
		QAction *a_propos_de_qet;
		QAction *a_propos_de_qt;
		QAction *configurer;
		QAction *entrer_pe;
		QAction *sortir_pe;
		QAction *toggle_aa;
		QAction *f_mosaique;
		QAction *f_cascade;
		QAction *f_reorganise;
		QAction *f_prec;
		QAction *f_suiv;
		
		void actions();
		// menus variables
		QAction *menu_systray_masquer_restaurer;
		
		private:
		QWorkspace workspace;
		SchemaVue *schemaEnCours();
		QSignalMapper windowMapper;
		/// dock for the panel of appliances
		QDockWidget *qdw_pa;
		/// Device Panel
		PanelAppareils *pa;
		/// Menu Elements for the Systray Icons
		QMenu *menu_systray;
		QAction *systray_masquer;
		QAction * config_fullscreen;
		QAction *systray_quitter;
		QMenu *menu_fenetres;
		/// icon in the systray
		QSystemTrayIcon *qsti;
		/// Geometry of the main window
		QByteArray wg;
		void menus();
		void toolbar();
		QToolBar *barre_outils;
		
		private slots:
		void slot_couper();
		void slot_copier();
		void slot_coller();
		void slot_zoomPlus();
		void slot_zoomMoins();
		void slot_zoomFit();
		void slot_zoomReset();
		void slot_selectAll();
		void slot_selectNothing();
		void slot_selectInvert();
		void slot_supprimer();
		void slot_pivoter();
		void slot_setSelectionMode();
		void slot_setVisualisationMode();
		void slot_updateActions();
		void slot_updateMenuFenetres();
	};
#endif
