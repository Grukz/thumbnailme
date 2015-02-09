/***************************************************************************//**
*	@brief Thumbnail me 3.0
*	Thumbnail me is a user interface for Movie thumbnailer.
* 	Generate thumbnails from any movie is now easier !
*
*	@file	ThumbnailListwidget.h
*       @class  ThumbnailListwidget
*	QListWidget de ThumbnailItem.
*
*	@author Quentin Rousseau\n
*	@note   Copyright (C) 2011-2012 Quentin Rousseau\n
*               License: GNU General Public License version 2 (GPLv2) - http://www.gnu.org/licenses/gpl-2.0.html\n
*               Site web: www.thumbnailme.com\n
*               Email: quentin.rousseau@thumbnailme.com
*
*       @since      3.0
*	@version    3.0
*       @date       2011-2012
*******************************************************************************/

#ifndef THUMBNAILLISTWIDGET_H
#define THUMBNAILLISTWIDGET_H

#include "defines.h"
#include "libQt+.h"
#include "ThumbnailItem.h"

#include <QListWidget>
#include <QLinkedList>
#include <QMessageBox>
#include <QKeyEvent>

class ThumbnailListWidget : public QListWidget
{
    Q_OBJECT

    public:
    explicit ThumbnailListWidget(QWidget * parent = 0);
    virtual ~ThumbnailListWidget();

    protected:
    void keyPressEvent ( QKeyEvent * event );

    public:
    void addItem  (QString item);
    void addItem  (ThumbnailItem* item);
    void addItems (QStringList items);
    QLinkedList <ThumbnailItem*> getItemsListWidget();

    public slots:
    void removeItemSelection();

    signals:
    /**
    *@brief Signal �mis lorsqu'un item est supprim� de la liste.
    */
    void itemDeleted();
};

#endif // THUMBNAILLISTWIDGET_H
