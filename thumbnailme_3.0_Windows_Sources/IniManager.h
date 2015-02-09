/***************************************************************************//**
*	@brief Thumbnail me 3.0
*	Thumbnail me is a user interface for Movie thumbnailer.
* 	Generate thumbnails from any movie is now easier !
*
*	@file	IniManager.h
*       @class  IniManager
*	Cette classe g�re toutes les lectures/�critures dans le fichier de configuration.
*       Elle applique �galement les param�tres lus dans le fichier de configuration aux diff�rents Docks.
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

#ifndef HEADER_INIMANAGER
#define HEADER_INIMANAGER

#include "MainWindow.h"
#include "libQt+.h"

class MainWindow;

class IniManager : public QWidget
{
    Q_OBJECT

    public:
    explicit IniManager(MainWindow *main_window);
    virtual ~IniManager();

    private:
    MainWindow *main_window;
    QSettings  *settings;
    QString    currentFileLoaded;

    private:
    void retranslate();

    protected:
    void changeEvent(QEvent* event);

    public:
    void            loadIni(QSettings * settings);
    void            registerIni(QSettings *settings);
    void            setCurrentFileLoaded(QString newFileLoaded);
    QString         getCurrentFileLoaded();

    public slots:
    void    importConf();
    void    loadRecentSettings1();
    void    loadRecentSettings2();
    void    loadRecentSettings3();
    void    loadRecentSettings4();
    void    loadRecentConfs();
    void    saveSettings();
    QString saveSettingsUnder();
};

#endif // HEADER_INIMANAGER
