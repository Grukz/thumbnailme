/***************************************************************************//**
*	@brief Thumbnail me 3.0
*	Thumbnail me is a user interface for Movie thumbnailer.
* 	Generate thumbnails from any movie is now easier !
*
*	@file	ThumbnailEngine.cpp
*       @class  ThumbnailEngine
*	Cette classe est le moteur de ThumbnailMe, c'est dans cette classe que le processus est ex�cut�.
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

#include "ThumbnailEngine.h"

/**
*@brief Constructeur.
*@param *main_window    Fen�tre principale de Thumbnail me.
*/
ThumbnailEngine::ThumbnailEngine(MainWindow *main_window)
{
    this->main_window = main_window;
    this->currentItem = 0;
    convertSignalMapper = new QSignalMapper(this);
    settings = new QSettings(QSettings::IniFormat,DEFAULT_PATH_INI,APPLICATION_NAME,DEFAULT_FILE_INI,this);

    process = new QProcess(this);
        process->setProcessChannelMode(QProcess::MergedChannels);

    connect( process, SIGNAL(readyReadStandardOutput ()), this ,  SLOT(buildOutput()) );
    connect( process, SIGNAL(finished(int)), convertSignalMapper, SLOT(map()) );
    connect( process, SIGNAL(finished(int)), this,                SLOT(detectShortDuration()) );
    connect( convertSignalMapper, SIGNAL(mapped(QObject *)),this, SLOT(convertToFormat(QObject *)) );
    connect( this, SIGNAL(itemTooShortDuration (ThumbnailItem *)), this , SLOT(successDialogItemRemove(ThumbnailItem *)) );
}

/**
*@brief Destructeur.
*/
ThumbnailEngine::~ThumbnailEngine()
{
    delete main_window;
    delete img;
    delete currentItem;
}

/**
*@brief  Appelle le processus "mtn".
*/
void ThumbnailEngine::start(int exitCode, QProcess::ExitStatus exitStatus)
{
    currentOutput.clear();

    if(!listInputFile.isEmpty())
    {
        QEventLoop loop;
        connect( process, SIGNAL(finished(int)), &loop, SLOT(quit()));

        current++;
        currentItem = this->listInputFile.takeFirst();
        main_window->processStatusBar->setStatus(current,currentItem->getFilePath().toString());

        connect( process,SIGNAL(finished(int, QProcess::ExitStatus)), this,SLOT(start(int, QProcess::ExitStatus)) );
        convertSignalMapper->setMapping(process,currentItem);

        process->start(DEFAULT_PATH_MTN, buildParams(currentItem));
        loop.exec();
        process->waitForFinished(-1);
    }
    else
    {
        this->success();
        disconnect( process,SIGNAL(finished(int, QProcess::ExitStatus)), this,SLOT(start(int, QProcess::ExitStatus)) );
    }
}

/**
*@brief  V�rifie si le binaire "mtn" existe bien.
*@return bool - Vrai ou faux.
*/
bool ThumbnailEngine::isBinaryExists()
{
    return QFile::exists(DEFAULT_PATH_MTN);
}

/**
*@brief Lance le processus de g�n�ration de vignette.
*@param mode    Mode de conversion.
*/
void ThumbnailEngine::run(int mode)
{
   switch (mode)
   {
     case(1):this->modeConversion = SIMPLEMOD;break;
     case(2):this->modeConversion = PREVIEWMOD;break;
   }

    if (modeConversion == SIMPLEMOD)
    {
        if (main_window->mpDockInputOutput->getListWidget()->count() != 0)
        {
            if(!main_window->mpDockInputOutput->isSameSourceChecked())
            {
                if (!main_window->mpDockInputOutput->getPathOutput().isEmpty())
                    if(QDir(main_window->mpDockInputOutput->getPathOutput()).exists())
                        if(!main_window->mpDockStyles->isNoFontDefined())
                          if(isBinaryExists())
                            launchProcess(main_window->mpDockInputOutput->getThumbnailList());
                          else QMessageBox::critical((QWidget *) this->parent(),_ERROR_,tr("Mtn binary not found. Operation cancelled."));
                        else QMessageBox::warning(main_window,_WARNING_,tr("No fonts defined"));
                    else QMessageBox::warning(main_window,_ERROR_,tr("Output path does not exist"));
               else QMessageBox::warning(main_window,_WARNING_,tr("Select an output folder, please"));
            }
            else
            {
                if(!main_window->mpDockStyles->isNoFontDefined())
                  if(isBinaryExists())
                    launchProcess(main_window->mpDockInputOutput->getThumbnailList());
                  else QMessageBox::critical((QWidget *) this->parent(),_ERROR_,tr("Mtn binary not found. Operation cancelled."));
                else QMessageBox::warning(main_window,_WARNING_,tr("No fonts defined"));
            }
        }
        else QMessageBox::warning(main_window,_WARNING_,tr("No file(s) loaded"));

    }
    else if (modeConversion == PREVIEWMOD)
    {
        if (!main_window->mpDockInputOutput->getListWidget()->selectedItems().isEmpty())
            if(!main_window->mpDockStyles->isNoFontDefined())
                if(isBinaryExists())
                {
                    QLinkedList <ThumbnailItem*> l;
                    l.append(main_window->mpDockInputOutput->getCurrentItem());
                    launchProcess(l);
                }
                else QMessageBox::critical((QWidget *) this->parent(),_ERROR_,tr("Mtn binary not found. Operation cancelled."));
            else QMessageBox::warning(main_window,_WARNING_,tr("No fonts defined"));
        else QMessageBox::warning(main_window,_WARNING_,tr("No file selected"));
    }
}

QStringList ThumbnailEngine::buildParams(ThumbnailItem *item)
{
    QStringList params;
    /*Fichier � transcoder*/
    params << item->getFilePath().toString();

      /***************/
     /*Config Params*/
    /***************/
    params << "-P" << "-h 0" <<"-c"
    << main_window->mpDockConf->getColumns() << "-r" << main_window->mpDockConf->getRows()
    << "-w" << main_window->mpDockConf->getWidth() <<"-g" << main_window->mpDockConf->getGap()
    << "-j" << "100" << "-b" << main_window->mpDockConf->getBlankSkip()
    << "-D" << main_window->mpDockConf->getEdgeDetect() <<"-L" << main_window->mpDockStyles->getInfoTextLocation()+":"+main_window->mpDockStyles->getTimeStampLocation()
    << "-k" << qColor2rgbNoSharp(main_window->mpDockStyles->getColorBackground()) << "-f" << main_window->mpDockStyles->getFontInfoText(0);

    QString colorFontsInfos = qColor2rgbNoSharp(main_window->mpDockStyles->getColorInfoText()) + ":" + main_window->mpDockStyles->getSizeInfoText();

    if(main_window->mpDockStyles->isTimeStampChecked())
         #if !defined(Q_OS_LINUX)
             colorFontsInfos.append(":" + main_window->mpDockStyles->getFontTimeStamp(2) + ":" + qColor2rgbNoSharp(main_window->mpDockStyles->getColorTimeStamp()) + ":" + qColor2rgbNoSharp(main_window->mpDockStyles->getColorShadow()) + ":" + main_window->mpDockStyles->getSizeTimeStamp());
         #else
             colorFontsInfos.append(":" + main_window->mpDockStyles->getFontTimeStamp(0) + ":" + qColor2rgbNoSharp(main_window->mpDockStyles->getColorTimeStamp()) + ":" + qColor2rgbNoSharp(main_window->mpDockStyles->getColorShadow()) + ":" + main_window->mpDockStyles->getSizeTimeStamp());
         #endif
     else params << "-t";

    params << "-F" << colorFontsInfos;
    params << "-o" << DEFAULT_TMP_EXTENSION << "-O" << QDir::tempPath();

    if(main_window->mpDockStyles->isInfoTextChecked() == false) params << "-i";
    if(main_window->mpDockStyles->getTitleEdit() != NULL) params << "-T" << main_window->mpDockStyles->getTitleEdit();

      /*****************/
     /*Timeline Params*/
    /*****************/
    if(item->isReadable() && !item->getLowerTime().isNull() && !item->getUpperTime().isNull())
    {
        params << "-B" << item->getBeginOmmitSecs() << "-E" << item->getEndOmmitSecs();
    }

    return params;
}

/**
*@brief Lance le processus.
*@param listInputFile    Liste des fichiers � vignetter.
*/
void ThumbnailEngine::launchProcess(QLinkedList <ThumbnailItem*> listInputFile)
{
    //Disable Docks - Security
    this->main_window->mpDockInputOutput->setDisabled(true);
    this->main_window->mpDockTimeline->setDisabled(true);

    //Init
    this->listInputFile =  QLinkedList <ThumbnailItem*> (listInputFile);
    this->current = 0;
    this->initSuccessDialog(QLinkedList<ThumbnailItem*> (this->listInputFile));

    disconnect( process, SIGNAL(finished(int)), this, SLOT(success()) );

    //Affichage de la status bar
    this->main_window->processStatusBar->setFilesCount(listInputFile.count());
    this->main_window->statusBar()->show();

    //Start process
    start(0,QProcess::NormalExit);
}

/**
*@brief D�termine le chemin absolu d'un fichier selon ses param�tres.
*@param pathOutput           Chemin de sortie du fichier.
*@param absolutePathInput    Chemin absolu du fichier d'entr�e.
*@param suffix               Suffixe du fichier.
*@param format               Format du fichier.
*/
QString ThumbnailEngine::absoluteFilePathOutput(QString pathOutput, QString absolutePathInput,QString suffix,QString format)
{
    QString absolutPathOutput;

    QFileInfo fileInfo (QDir::toNativeSeparators(absolutePathInput));
    if (!format.isEmpty())
      absolutPathOutput = QDir::toNativeSeparators(pathOutput + "/" + fileInfo.fileName().replace("."+fileInfo.suffix(),suffix+"."+format));
    else absolutPathOutput = QDir::toNativeSeparators(pathOutput + "/" + fileInfo.fileName().replace("."+fileInfo.suffix(),suffix));
    return absolutPathOutput.replace("\\\\","\\").replace("//","/");
}

/**
*@brief  Apelle la fen�tre de succ�s.
*/
void ThumbnailEngine::initSuccessDialog(QLinkedList <ThumbnailItem*> listInputFile)
{
    main_window->mpSuccessDialog->clearListWidget();

    if(!main_window->mpDockInputOutput->isSameSourceChecked() && this->mode() == SIMPLEMOD)
    {
        main_window->mpSuccessDialog->setPathOpenFolder(main_window->mpDockInputOutput->getPathOutput());

        foreach(ThumbnailItem *item, listInputFile)
        {
            item->setFilePathOutput(QUrl(absoluteFilePathOutput(main_window->mpDockInputOutput->getPathOutput(),item->getFilePath().toString(),settings->value("Extras/outputSuffix").toString(),main_window->mpDockConf->getFormatFile())));
            main_window->mpSuccessDialog->addItem(item);
        }
    }
    else if(main_window->mpDockInputOutput->isSameSourceChecked() && this->mode() == SIMPLEMOD)
    {
        main_window->mpSuccessDialog->setPathOpenFolder(QString());
        foreach(ThumbnailItem *item, listInputFile)
        {
            item->setFilePathOutput(QUrl(absoluteFilePathOutput(QDir::toNativeSeparators(QFileInfo(item->getFilePath().toString()).canonicalPath()),item->getFilePath().toString(),settings->value("Extras/outputSuffix").toString(),main_window->mpDockConf->getFormatFile())));
            main_window->mpSuccessDialog->addItem(item);
        }
    }
}

/**
*@brief Converti le fichier cr�es temporairement dans le format et la qualit� s�lectionn� par l'utilisateur.
*@param o   Item � convertir.
*/
void ThumbnailEngine::convertToFormat(QObject *o)
{
    ThumbnailItem *item = qobject_cast<ThumbnailItem*>(o);

    QString suffix = settings->value("Extras/outputSuffix").toString();

    QString path = absoluteFilePathOutput(QDir::tempPath(),item->getFilePath().toString(),DEFAULT_TMP_EXTENSION);

    QString output;
    if (modeConversion == SIMPLEMOD && !main_window->mpDockInputOutput->isSameSourceChecked())
        output = main_window->mpDockInputOutput->getPathOutput() + "/" + QFileInfo(path).completeBaseName() + suffix +"."+ main_window->mpDockConf->getFormatFile();
    else if (modeConversion == SIMPLEMOD && main_window->mpDockInputOutput->isSameSourceChecked())
        output = QDir::toNativeSeparators(QFileInfo(currentItem->getFilePath().toString()).canonicalPath() + "/")+ QFileInfo(path).completeBaseName() + suffix + "."+ main_window->mpDockConf->getFormatFile();

    img = new QImage(path);
        img->save(output,main_window->mpDockConf->getFormatFile().toAscii().constData(),main_window->mpDockConf->getQuality().toInt());
}


/**
*@brief   Supprimer les fichiers temporaires g�n�r�s par le moteur.
*@remarks DEFAULT_TMP_EXTENSION d�finit dans defines.h.
*/
void ThumbnailEngine::deleteTemporaryFiles()
{
    QDir tmpFolder = QDir::tempPath();
    QStringList filters;
    filters << "*"+DEFAULT_TMP_EXTENSION;
    tmpFolder.setNameFilters(filters);

    QFileInfoList temporaryFiles = tmpFolder.entryInfoList();

    while (!temporaryFiles.isEmpty())
    {
        QFile::remove(temporaryFiles.first().absoluteFilePath());
        temporaryFiles.removeFirst();
    }
}

/**
*@brief Change le mode du moteur.
*@param mode    Mode du moteur.
*/
void ThumbnailEngine::setMode(Mode mode)
{
    modeConversion = mode;
}

/**
*@brief  Retourne le mode du moteur.
*@return mode - Mode du moteur.
*/
ThumbnailEngine::Mode ThumbnailEngine::mode() const
{
    return this->modeConversion;
}

/**
*@brief Construit le QStringList Output.
*/
void ThumbnailEngine::buildOutput()
{
    QString outputString = process->readAllStandardOutput().trimmed();
    currentOutput << outputString.split(QString("\n"),QString::SkipEmptyParts);
}

/**
*@brief D�tecte si le fichier est trop court pour �tre trait� par Mtn.
*@brief Trop court si Mtn g�n�re moins de 4 lignes.
*/
void ThumbnailEngine::detectShortDuration()
{
    this->main_window->mpVerboseWindow->setVerbose(currentOutput);

    if (currentOutput.count() <= 4)
    {
        this->main_window->mpVerboseWindow->setVerbose("<font color=\"red\">Error: The duration of this file seems too short.</font>");
        emit itemTooShortDuration(this->currentItem);
    }
}

/**
*@brief Retire les items non trait� de la successDialog car trop court via le signal itemTooShortDuration.
*@param item - Item � retirer.
*/
void ThumbnailEngine::successDialogItemRemove(ThumbnailItem * item)
{
    main_window->mpSuccessDialog->removeItem(item);
}

/**
*@brief  Envoie une image en preview.
*/
void ThumbnailEngine::success()
{
    //1 Hide Status Bar
        main_window->processStatusBar->hide();

    //2 Re Enable Docks
        this->main_window->mpDockInputOutput->setEnabled(true);

        if(this->main_window->mpDockTimeline->getCurrentItem() != NULL && this->main_window->mpDockTimeline->getCurrentItem()->isReadable())
        this->main_window->mpDockTimeline->setEnabled(true);

    //3 Preview
        QString suffix = settings->value("Extras/outputSuffix").toString();

        if (modeConversion == SIMPLEMOD && !main_window->mpDockInputOutput->isSameSourceChecked())
            main_window->mpPreviewGraphicView->setPreview(absoluteFilePathOutput(main_window->mpDockInputOutput->getPathOutput(),currentItem->getFilePath().toString(),suffix,main_window->mpDockConf->getFormatFile()));
        else if (modeConversion == SIMPLEMOD && main_window->mpDockInputOutput->isSameSourceChecked())
            main_window->mpPreviewGraphicView->setPreview(absoluteFilePathOutput(QDir::toNativeSeparators(QFileInfo(currentItem->getFilePath().toString()).canonicalPath()),currentItem->getFilePath().toString(),suffix,main_window->mpDockConf->getFormatFile()));
        else if (modeConversion == PREVIEWMOD)
            main_window->mpPreviewGraphicView->setPreview(absoluteFilePathOutput(QDir::tempPath(),currentItem->getFilePath().toString(),DEFAULT_TMP_EXTENSION));

    //4 Show SuccessDialog
        if(modeConversion == SIMPLEMOD && main_window->mpSuccessDialog->getListWidget()->count() != 0)
            main_window->mpSuccessDialog->exec();
}
