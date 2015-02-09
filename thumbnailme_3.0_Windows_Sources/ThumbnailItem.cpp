/***************************************************************************//**
*	@brief Thumbnail me 3.0
*	Thumbnail me is a user interface for Movie thumbnailer.
* 	Generate thumbnails from any movie is now easier !
*
*	@file	ThumbnailItem.h
*       @class  ThumbnailItem
*	Repr�sente un item de type Thumbnail.
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

#include "ThumbnailItem.h"

/**
*@brief Constructeur.
*/
ThumbnailItem::ThumbnailItem(QUrl filePath) : QObject() , QListWidgetItem ()
{
    this->setText(filePath.toString());
    this->filePath = filePath;
    this->filePathOutput = QString();
    this->mediaPlayer = new QMediaPlayer(this);
    this->readable = true;

    connect(mediaPlayer, SIGNAL(durationChanged (qint64)),    this, SLOT(manageDuration(qint64)));
    connect(mediaPlayer, SIGNAL(error (QMediaPlayer::Error)), this, SLOT(manageError(QMediaPlayer::Error)));

    mediaPlayer->setMedia(QMediaContent(QUrl::fromLocalFile(filePath.toString())));
    mediaPlayer->pause();
}

/**
*@brief Constructeur de copie.
*/
ThumbnailItem::ThumbnailItem(ThumbnailItem &other) : QObject() , QListWidgetItem (other)
{
    this->setText(other.text());
    this->filePath = other.getFilePath();
    this->filePathOutput = other.getFilePathOutput();
    this->readable = other.isReadable();
    this->lowerQTime = other.getLowerTime();
    this->upperQTime = other.getUpperTime();

    this->mediaPlayer = new QMediaPlayer(this);

    connect(mediaPlayer, SIGNAL(durationChanged (qint64)),    this, SLOT(manageDuration(qint64)));
    connect(mediaPlayer, SIGNAL(error (QMediaPlayer::Error)), this, SLOT(manageError(QMediaPlayer::Error)));

    mediaPlayer->setMedia(QMediaContent(QUrl::fromLocalFile(filePath.toString())));
    mediaPlayer->pause();
}

/**
*@brief Destructeur.
*/
ThumbnailItem::~ThumbnailItem()
{
}

/**
*@brief Setter du QTime de d�but.
*@param t  Qtime
*/
void ThumbnailItem::setLowerTime(QTime t)
{
    this->lowerQTime = t;
}

/**
*@brief Setter du QTime de fin.
*@param t  Qtime
*/
void ThumbnailItem::setUpperTime(QTime t)
{
    this->upperQTime = t;
}

/**
*@brief Setter du QTime de la dur�e totale.
*@param t  Qtime
*/
void ThumbnailItem::setTotalDuration(QTime t)
{
    this->totalDurationQTime = t;
}

/**
*@brief Setter du Fichier du chemin.
*@param path QUrl.
*/
void ThumbnailItem::setFilePath(QUrl path)
{
    this->filePath = path;
    this->setText(path.toString());
}

/**
*@brief Setter du Fichier du chemin de destination.
*@param path QUrl.
*/
void ThumbnailItem::setFilePathOutput(QUrl path)
{
    this->filePathOutput = path;
}

/**
*@brief  Retourne le QTime de d�but.
*@return QTime - QTime
*/
QTime ThumbnailItem::getLowerTime()
{
    return this->lowerQTime;
}

/**
*@brief  Retourne le QTime de fin.
*@return QTime - QTime
*/
QTime ThumbnailItem::getUpperTime()
{
    return this->upperQTime;
}

/**
*@brief  Retourne le QTime de la dur�e totale.
*@return QTime - QTime
*/
QTime ThumbnailItem::getTotalDuration()
{
    return this->totalDurationQTime;
}

/**
*@brief  Retourne le chemin de l'item.
*@return QUrl - Url du fichier
*/
QUrl ThumbnailItem::getFilePath()
{
    return filePath;
}

/**
*@brief  Retourne le chemin de destination de l'item.
*@return QUrl - Url du fichier
*/
QUrl ThumbnailItem::getFilePathOutput()
{
    return filePathOutput;
}

/**
*@brief  Retourne le nombre de secondes omis au d�but du m�dia.
*@return QString - Format X.X en secondes.
*/
QString ThumbnailItem::getBeginOmmitSecs()
{
    return QString::number(-this->getLowerTime().secsTo(QTime()));
}

/**
*@brief  Retourne le nombre de secondes omis � la fin du m�dia.
*@return QString - Format X.X en secondes.
*/
QString ThumbnailItem::getEndOmmitSecs()
{
    return QString::number(this->getUpperTime().secsTo(this->getTotalDuration()));
}

/**
*@brief R�cup�re la dur�e d'un Item.
*@param duration    Dur�e du m�dia en ms.
*/
void ThumbnailItem::manageDuration(qint64 duration)
{
    QTime t(0,0);
    this->totalDurationQTime = t.addMSecs(duration);

    if(!this->totalDurationQTime.isValid())
        readable = false;
}

/**
*@brief R�cup�ration de l'erreur si une a lieu.
*@param error   Type d'erreur.
*/
void ThumbnailItem::manageError(QMediaPlayer::Error error)
{
    readable = false;
}
