#ifndef COMMANDS_H
#define COMMANDS_H

#include <QUndoCommand>
#include <QMimeData>
#include <QPoint>
#include <QApplication>
#include "Table.h"

class DataChanged : public QUndoCommand
{
   public:
      DataChanged(Table*, int, int, const QString&, const QString&);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      int row;
      int column;
      QString oldData;
      QString newData;
};

class InsertRowsCommand : public QUndoCommand
{
   public:
      InsertRowsCommand(Table*, int, int);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      int amount;
      int position;
};

class InsertColumnsCommand : public QUndoCommand
{
   public:
      InsertColumnsCommand(Table*, int, int);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      int amount;
      int position;
};

class InsertImageCommand : public QUndoCommand
{
   public:
      InsertImageCommand(Table*, const QImage&, int, int);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      Graphic *graphic;
      int xCoordinate;
      int yCoordinate;
};

class DeleteContents : public QUndoCommand
{
   public:
      DeleteContents(Table*);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      QTableWidgetSelectionRange range;
      Graphic *graphic;
      QMimeData oldData;
};

class DeleteEntireRows : public QUndoCommand
{
   public:
      DeleteEntireRows(Table*, int, int);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      int position;
      int amount;
      QMimeData oldData;
};

class DeleteEntireColumns : public QUndoCommand
{
   public:
      DeleteEntireColumns(Table*, int, int);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      int position;
      int amount;
      QMimeData oldData;
};

class CutCommand : public QUndoCommand
{
   public:
      CutCommand(Table*);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      QMimeData oldData;
      Graphic *selectedGraphic;
      QTableWidgetSelectionRange selectedRange;
};

class PasteCommand : public QUndoCommand
{
   public:
      PasteCommand(Table*);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      Graphic *graphic;
      QMimeData oldData;
      QMimeData newData;
      int row;
      int column;
};

class ChangeCellsEditor : public QUndoCommand
{
   public:
      ChangeCellsEditor(Table*, QTableWidgetSelectionRange, int);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      QTableWidgetSelectionRange range;
      QList<int> oldEditors;
      int newEditor;
};

class ChangeCellsFont : public QUndoCommand
{
   public:
      ChangeCellsFont(Table*, QTableWidgetSelectionRange, QFont);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      QTableWidgetSelectionRange range;
      QList<QFont> oldFonts;
      QFont newFont;
};

class ChangeCellsAlignment : public QUndoCommand
{
   public:
      ChangeCellsAlignment(Table*, QTableWidgetSelectionRange, int);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      QTableWidgetSelectionRange range;
      QList<int> oldAlignments;
      int newAlignment;
};

class ChangeCellsBackground : public QUndoCommand
{
   public:
      ChangeCellsBackground(Table*, QTableWidgetSelectionRange, QColor);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      QTableWidgetSelectionRange range;
      QList<QColor> oldBackgrounds;
      QColor newBackground;
};

class ChangeCellsForeground : public QUndoCommand
{
   public:
      ChangeCellsForeground(Table*, QTableWidgetSelectionRange, QColor);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      QTableWidgetSelectionRange range;
      QList<QColor> oldForegrounds;
      QColor newForeground;
};

class SortCommand : public QUndoCommand
{
   public:
      SortCommand(Table*, const TableSort&);
      virtual void undo();
      virtual void redo();
      
   private:
      Table *table;
      QMimeData oldData;
      QTableWidgetSelectionRange range;
      TableSort tableSort;
};

class GraphicDraggedCommand : public QUndoCommand
{
   public:
      GraphicDraggedCommand(Graphic*, QPoint, QPoint);
      virtual void undo();
      virtual void redo();
      
   private:
      Graphic *graphic;
      QPoint oldPosition;
      QPoint newPosition;
};

class GraphicResizedCommand : public QUndoCommand
{
   public:
      GraphicResizedCommand(Graphic*, QSize, QSize, Graphic::BoundaryPosition);
      virtual void undo();
      virtual void redo();
      
   private:
      Graphic *graphic;
      QSize oldSize;
      QSize newSize;
      Graphic::BoundaryPosition boundaryPosition;
};

#endif
