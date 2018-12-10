#ifndef EXCELMAINWINDOW_H
#define EXCELMAINWINDOW_H

#include <QMainWindow>
class Table;
class QLabel;
class QAction;
class FindDialog;
class QLineEdit;
class QMessageBox;
class FunctionListDialog;
class Graphic;
class SortDialog;

namespace excel {

class MainWindow : public QMainWindow
{
   Q_OBJECT
   
   public:
      MainWindow(QWidget *parent);
	  ~MainWindow();
      QAction *getUndo();
      QAction *getRedo();
      QAction *getAlignLeft();
      QAction *getAlignRight();
      QAction *getAlignCenter();
      
      
Q_SIGNALS:
      void askUpdateCurrentLabelCell(const QString &text);
      
   protected:
      void closeEvent(QCloseEvent*);
      
   public slots:
      bool newFile();
      bool open(QString = QString());
      bool save();
      bool saveAs();
      
      void undo();
      void redo();

      void cut();
      void copy();
      void paste();
      void find();
      void findNext();
      void findPrevious();
      void deleteContents();
      void deleteEntireRows();
      void deleteEntireColumns();
      
      void insertRows();
      void insertColumns();
      void insertImage();
            
      void textLineFormat();
      void dateFormat();
      void timeFormat();
      void integerFormat();
      void decimalFormat();
      void font();
      void align(bool);
      
      void alignleft();
      void alignright();
      void aligncenter();
      
      
      void background();
      void foreground();
      
      void sort();
      
      void aboutQSimpleSheet();
      void functionList();
      
      void userChangedItemDataSlot(int, int, QString&, QString&);
      void graphicContextMenuSlot(QPoint);
      void documentModified();
      void updateStatusBar();
      void updateFormulaToolBar();
      void currentCellLineEditReturnPressed();
      void formulaLineEditReturnPressed();
      void updateActions();
      
   private:
      Table *table;
      FindDialog *findDialog;
      QString currentFile;
      QLineEdit *formulaLineEdit;
      QLineEdit *currentCellLineEdit;
      QLabel *currentCellDataLabel;
      FunctionListDialog *functionListDialog;
      
      ///ACTIONS
      QAction *newAction;
      QAction *openAction;
      QAction *saveAction;
      QAction *saveAsAction;
      QAction *exitAction;
      
      QAction *undoAction;
      QAction *redoAction;
      QAction *copyAction;
      QAction *cutAction;
      QAction *pasteAction;
      QAction *findAction;
      QAction *findNextAction;
      QAction *findPreviousAction;
      QAction *deleteContentsAction;
      QAction *deleteEntireRowsAction;
      QAction *deleteEntireColumnsAction;
      QAction *selectCurRowAction;
      QAction *selectCurColAction;
      QAction *selectAllAction;
      
      QAction *insertRowsAction;
      QAction *insertColumnsAction;
      QAction *insertImageAction;
      
      QAction *textLineFormatAction;
      QAction *dateFormatAction;
      QAction *timeFormatAction;
      QAction *integerFormatAction;
      QAction *decimalFormatAction;
      QAction *fontAction;
      QAction *alignLeftAction;
      QAction *alignCenterAction;
      QAction *alignRightAction;
      QAction *backgroundAction;
      QAction *foregroundAction;
      
      QAction *sortAction;
      
      QAction *showGridAction;
      
      QAction *functionListAction;
      QAction *aboutQSimpleSheetAction;
      QAction *aboutQtAction;
      
      ///Menus
      QMenu *fileMenu;
      QMenu *editMenu;
      QMenu *insertMenu;
      QMenu *formatMenu;
      QMenu *toolsMenu;
      QMenu *optionsMenu;
      QMenu *helpMenu;
      
      ///Toolbars
      QToolBar *mainToolBar;
      QToolBar *formulaToolBar;
      
      ///Private Functions
      void creatActions();
      void creatMenus();
      void creatToolBars();
      void creatFormulaToolBar();
      void creatStatusBar();
      void creatContextMenu();
      bool askToSaveChanges();
      bool saveFile(const QString&);
      void setCurrentFile(const QString&);
};
}
#endif



