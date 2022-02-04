/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#define HAVE_CUBE_NETWORKING

#include "config.h"

#include "MainWidget.h"

#include <QString>
#include <QPixmap>
#include <QHBoxLayout>
#include <QAction>
#include <QWhatsThis>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QScrollArea>
#include <QProgressDialog>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QApplication>
#include <QVariant>

#include "CubeParseContext.h"
#include "CubeError.h"
#include "CubeServices.h"
#include "CubeTypes.h"
#include "CubeProxy.h"
#include "CubePlatformsCompat.h"
#include "CubeUrl.h"
#include "CubeOperationProgress.h"
#include "CubeIoProxy.h"
#include "CubeNetworkProxy.h"
#include "CubeQtStreamSocket.h"

#include "Globals.h"
#include "PluginManager.h"
#include "PluginServices.h"
#include "Constants.h"
#include "Settings.h"
#include "TabManager.h"
#include "DimensionOrderDialog.h"
#include "TreeView.h"
#include "MetricTree.h"
#include "ColorScale.h"
#include "ColorMap.h"
#include "ContextFreeServices.h"
#include "PluginServices.h"
#include "PrecisionWidget.h"
#include "DefaultColorMap.h"
#include "SynchronizationToolBar.h"
#include "ValueViewConfig.h"
#include "DefaultValueView.h"
#include "HelpBrowser.h"
#include "RemoteFileDialog.h"
#include "StyleSheetEditor.h"
#include "TreeConfig.h"
#include "CubeApplication.h"
#include "CubeWriter.h"

#include <cassert>
#include <iostream>
// #ifndef CUBE_COMPRESSED
#include <fstream>
// #else
#include "CubeZfstream.h"
// #endif

#include "HmiInstrumentation.h"

#define FILE_HISTORY_COUNT 20

using namespace std;
using namespace cubegui;
using namespace cubepluginapi;

MainWidget::MainWidget( CubeApplication& _app, cube::CubeStrategy _strategy ) : app( _app ), strategy( _strategy )
{
    cube                 = NULL;
    lastExternalFileName = "";
    cubeExternal         = NULL;
    dynloadThreshold     = 1000000000;
    _initGeometry        = false;
    lastColorMapName     = "";
    fileLoaded           = false;
    recentFileWidget     = new QScrollArea();
    settings             = new Settings();

    setUpdatesEnabled( false );

    Globals::settings = settings;
    resize( 800, 600 );

    this->setAcceptDrops( true );

    cube::Socket::setSocketFactory( cube::QtStreamSocket::create );

    setWindowIcon( QIcon( ":images/CubeIcon.xpm" ) );

    stackedWidget = new QStackedWidget();
    stackedWidget->setWindowIcon( QIcon( ":images/CubeIcon.xpm" ) );

    StatusBarWidget* central = new StatusBarWidget();
    statusBar = central->getStatusBar();
    central->setWidget( stackedWidget );
    setCentralWidget( central );

    tabManager = new TabManager();
    tabManager->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    colorScale = new ColorScale();
    colorScale->setVisible( false );

    QWidget* mainWidget = new QWidget();
    mainWidget->setObjectName( "main" );

    QVBoxLayout* mainLayout = new QVBoxLayout();
    // LAYOUT_MARGIN and LAYOUT_SPACING are defined in constants.h
    mainLayout->setContentsMargins( LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN );
    mainLayout->setSpacing( LAYOUT_SPACING );
    mainWidget->setLayout( mainLayout );
    mainLayout->addWidget( tabManager );
    mainLayout->addWidget( colorScale );

    Globals::initialize( this, tabManager );

    PrecisionWidget* precisionWidget = Globals::getPrecisionWidget();
    connect( precisionWidget, SIGNAL( apply() ), tabManager, SLOT( updateTreeItemProperties() ) );

    /* initialize cube dependent plugins */
    PluginManager::getInstance()->setMainWindow( this );

    /* initialize context free plugins */
    ContextFreeServices* cfs = ContextFreeServices::getInstance();
    //QWidget*             initialScreen = new QWidget( this );
    initialScreen = new WidgetWithBackground();
    QWidget* contextFree = new QWidget( this );
    cfs->setWidget( contextFree );
    connect( cfs, SIGNAL( openCubeRequest( cube::CubeProxy* ) ), this, SLOT( openCube( cube::CubeProxy* ) ) );

    stackedWidget->addWidget( mainWidget );    // loaded cube (CONTEXT_CUBE)
    stackedWidget->addWidget( initialScreen ); // start screen (CONTEXT_INIT)
    stackedWidget->addWidget( contextFree );   // context free plugin (CONTEXT_FREE)
    stackedWidget->addWidget( new QWidget() ); // CONTEXT_EMPTY
    stackedWidget->setCurrentIndex( CONTEXT_INIT );

    syncToolBar = new SynchronizationToolBar();
    syncToolBar->setVisible( false );
    addToolBar( syncToolBar );

    // menu creation
    // note: system tab widget must be created before createMenu() gets called
    createMenu();

    setWhatsThis( CUBEGUI_FULL_NAME + tr( " is a presentation component suitable for displaying performance data for parallel programs including MPI and OpenMP applications. Program performance is represented in a multi-dimensional space including various program and system resources. The tool allows the interactive exploration of this space in a scalable fashion and browsing the different kinds of performance behavior with ease. Cube also includes a library to read and write performance data as well as operators to compare, integrate, and summarize data from different experiments." ) );

    menuBar()->setWhatsThis( tr( "The menu bar consists of three menus, a file menu, a display menu, and a help menu. Some menu functions have also a keyboard shortcut, which is written beside the menu item's name in the menu. E.g., you can open a file with Ctrl+O without going into the menu.  A short description of the menu items is visible in the status bar if you stay for a short while with the mouse above a menu item." ) );

    addToolBar( settings->getToolBar() );

    settings->registerSettingsHandler( this );
    settings->registerSettingsHandler( dynamic_cast<InternalSettingsHandler*> ( Globals::defaultColorMap ) );
    settings->registerSettingsHandler( dynamic_cast<InternalSettingsHandler*> ( Globals::defaultValueView ) );
    settings->registerSettingsHandler( PluginManager::getInstance() );

    PluginManager::getInstance()->initializePlugins(); // requires settings to store plugin path
    createInitialScreen( initialScreen );
    setColorMap( Globals::getColorMap() );

    tabManager->setFocusPolicy( Qt::StrongFocus );
    tabManager->setFocus();

    // set all splitter childs to same size
    int        width = this->width();
    QList<int> sizes;
    sizes << width / 3 << width / 3 << width / 3;
    tabManager->setSizes( sizes );
    settings->loadGlobalSettings();
    setUpdatesEnabled( true );
}
// end of constructor

void
MainWidget::setContext( const CubeContext& context )
{
    stackedWidget->setCurrentIndex( context );
}

void
MainWidget::createInitialScreen( QWidget* mainWidget )
{
    //======== right part: choose cube input file
    QVBoxLayout* vbox = new QVBoxLayout();

    QPushButton* open = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_DirOpenIcon ), tr( " Open Cube File " ) );
    QFont        font = open->font();
    font.setPointSize( font.pointSize() + 2 );
    open->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    connect( open, SIGNAL( clicked( bool ) ), this, SLOT( openFile() ) );
    open->setFont( font );

    // recent cube files ---------------
    QLabel* label = new QLabel( tr( "Recent Cube Files" ) );
    label->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Fixed );
    vbox->addWidget( open );
    vbox->addSpacing( 10 );
    vbox->addWidget( label );
    vbox->addWidget( recentFileWidget );

    MoveableWidget* fileWidget = new MoveableWidget();
    fileWidget->setLayout( vbox );
    fileWidget->setAutoFillBackground( true ); // don't fill with parents background
    fileWidget->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
    fileWidget->disableMoving( recentFileWidget );
    fileWidget->disableMoving( open );

    // spacing/layout
    QVBoxLayout* vboxOut   = new QVBoxLayout();
    QWidget*     widgetOut = new QWidget();
    widgetOut->setLayout( vboxOut );
    vboxOut->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    vboxOut->addWidget( fileWidget );
    vboxOut->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

    //======== left part: context free plugins
    QVBoxLayout*    contextL    = new QVBoxLayout();
    MoveableWidget* contextFree = new MoveableWidget();
    contextFree->setLayout( contextL );
    contextFree->setAutoFillBackground( true ); // don't fill with parents background
    contextFree->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );

    QList<QAction*> actions = PluginManager::getInstance()->getContextFreeActions();
    if ( actions.size() > 0 )
    {
        contextL->addWidget( new QLabel( tr( "Open context free plugin:" ) ) );

        foreach( QAction * action, actions )
        {
            QToolButton* button = new QToolButton();
            button->setDefaultAction( action );
            contextL->addWidget( button );
            contextFree->disableMoving( button );
        }
        QScrollArea* scroll = new QScrollArea();
        scroll->setWidget( contextFree );
    }
    // spacing/layout
    QVBoxLayout* vboxOut2   = new QVBoxLayout();
    QWidget*     widgetOut2 = new QWidget();
    widgetOut2->setLayout( vboxOut2 );
    vboxOut2->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
    vboxOut2->addWidget( contextFree );
    vboxOut2->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

    // main layout
    QHBoxLayout* main = new QHBoxLayout();
    mainWidget->setLayout( main );
    main->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
    main->addWidget( contextFree );
    main->addSpacing( 50 );
    main->addWidget( fileWidget );
    main->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );
}

void
MainWidget::recentFileSelected( const QString& link )
{
    int idx = openedFiles.indexOf( link );
    if ( idx >= 0 )
    {
        QAction* action = lastFileAct.at( idx );
        action->trigger();
    }
}

void
MainWidget::recentFileHovered( int idx )
{
    if ( idx >= 0 )
    {
        QString message = openedFiles[ idx ];
        if ( !message.startsWith( "cube://" ) ) // local file -> check for ini-file
        {
            // read preview image from settings
            QFileInfo file( openedFiles[ idx ] );
            QString   iniFile = QString( cube::services::get_cube_name( file.absoluteFilePath().toStdString() ).c_str() ).append( ".ini" );
            if ( QFile( iniFile ).exists() )
            {
                QSettings experimentSettings( iniFile, QSettings::IniFormat );
                experimentSettings.beginGroup( "lastState" );
                QPixmap pixmap    = experimentSettings.value( "image" ).value<QPixmap>();
                int     locations = experimentSettings.value( "locations" ).toInt();
                experimentSettings.endGroup();

                if ( locations > 0 )
                {
                    message += ",  #locations: " + QString::number( locations );
                }
                initialScreen->setImage( pixmap );
            }
        }
        setMessage( message, Information, false );
    }
    else
    {
        setMessage( "", Information, false );
        initialScreen->setImage( QPixmap() );
    }
}

MainWidget::~MainWidget()
{
    delete settings; // saves global settings on deletation
    delete cube;
}

/**********************************************************/
/****************** menu **********************************/
/**********************************************************/

// creates the pull-down menu
//
void
MainWidget::createMenu()
{
    // file menu

    fileMenu = menuBar()->addMenu( tr( "&File" ) );
    fileMenu->setStatusTip( tr( "Ready" ) );

    QAction* openAct = new QAction( tr( "&Open..." ), this );
    openAct->setShortcut( tr( "Ctrl+O" ) );
    openAct->setStatusTip( tr( "Opens a Cube file" ) );
    connect( openAct, SIGNAL( triggered() ), this, SLOT( openFile() ) );
    fileMenu->addAction( openAct );
    openAct->setWhatsThis( tr( "Offers a selection dialog to open a Cube file. In case of an already opened file, it will be closed before a new file gets opened. If a file got opened successfully, it gets added to the top of the recent files list (see below). If it was already in the list, it is moved to the top." ) );

#if defined ( HAVE_CUBE_NETWORKING )
    QAction* openremoteAct = new QAction( tr( "&Open URL..." ), this );
    openremoteAct->setShortcut( tr( "Shift+Ctrl+O" ) );
    openremoteAct->setStatusTip( tr( "Opens a file referenced as URL." ) );
    connect( openremoteAct, SIGNAL( triggered() ), this, SLOT( openRemote() ) );
    fileMenu->addAction( openremoteAct );
    openremoteAct->setWhatsThis( tr( "Offers an input field for a URL to open a remote file." ) );
#endif

    QMenu* contextFreeMenu = PluginManager::getInstance()->getContextFreePluginMenu();
    fileMenu->addMenu( contextFreeMenu );
    connect( this, SIGNAL( enableClose( bool ) ), contextFreeMenu, SLOT( setDisabled( bool ) ) );

    saveAsAct = new QAction( tr( "&Save as..." ), this );
    saveAsAct->setShortcut( tr( "Ctrl+S" ) );
    saveAsAct->setStatusTip( tr( "Save the current cube under another name..." ) );
    saveAsAct->setEnabled( false );
    connect( saveAsAct, SIGNAL( triggered() ), this, SLOT( saveAs() ) );
    fileMenu->addAction( saveAsAct );
    saveAsAct->setWhatsThis( tr( "Offers a selection dialog to select a new name for a Cube file." ) );


    QAction* closeAct = new QAction( tr( "&Close" ), this );
    closeAct->setShortcut( tr( "Ctrl+W" ) );
    closeAct->setStatusTip( tr( "Closes the file" ) );
    closeAct->setEnabled( false );
    connect( closeAct, SIGNAL( triggered() ), this, SLOT( closeFile() ) );
    connect( this, SIGNAL( enableClose( bool ) ), closeAct, SLOT( setEnabled( bool ) ) );
    fileMenu->addAction( closeAct );
    closeAct->setWhatsThis( tr( "Closes the currently opened Cube file. Disabled if no file is opened." ) );

    openExtAct = new QAction( tr( "Open &external..." ), this );
    openExtAct->setStatusTip( tr( "Opens a second cube file for external percentage" ) );
    connect( openExtAct, SIGNAL( triggered() ), this, SLOT( openExternalFile() ) );
    fileMenu->addAction( openExtAct );
    openExtAct->setWhatsThis( tr( "Opens a file for the external percentage value mode (see further help on the value modes)." ) );
    openExtAct->setEnabled( false );

    QAction* closeExtAct = new QAction( tr( "Close e&xternal" ), this );
    closeExtAct->setStatusTip( tr( "Closes the second cube file for external percentage" ) );
    closeExtAct->setEnabled( false );
    connect( closeExtAct, SIGNAL( triggered() ), this, SLOT( closeExternalFile() ) );
    connect( this, SIGNAL( enableExtClose( bool ) ), closeExtAct, SLOT( setEnabled( bool ) ) );
    connect( this, SIGNAL( enableExtClose( bool ) ), closeExtAct, SLOT( setEnabled( bool ) ) );
    fileMenu->addAction( closeExtAct );
    closeExtAct->setWhatsThis( tr( "Closes the current external file and removes all corresponding data. Disabled if no external file is opened." ) );

    fileMenu->addMenu( settings->getMenu() );
    fileMenu->addSeparator();

#ifdef WITH_DYNLOAD
    QAction* dynloadAct = new QAction( tr( "D&ynamic loading threshold..." ), this );
    dynloadAct->setStatusTip( tr( "Sets the threshold for dynamic loading of data" ) );
    connect( dynloadAct, SIGNAL( triggered() ), this, SLOT( setDynloadThreshold() ) );
    fileMenu->addAction( dynloadAct );
    dynloadAct->setWhatsThis( tr( "By default, Cube always loads the whole amount of data when you open a Cube file. However, Cube offers also a possibility to load only those data which is needed for the current display. To be more precise, the data for the selected metric(s) and, if a selected metric is expanded, the data for its children are loaded. If you change the metric selection, possibly some new data is needed for the display that is dynamically loaded on demand. Currently not needed data gets unloaded.\n\nThis functionality is useful most of all for large files. Under this menu item you can define a file size threshold (in bytes) above which Cube offers you dynamic data loading. If a file being opened is larger than this threshold, Cube will ask you if you wish dynamic loading." ) );
#endif

    QAction* screenshotAct = new QAction( tr( "Sc&reenshot..." ), this );
    screenshotAct->setStatusTip( tr( "Saves a screenshot into the file cube.png" ) );
    connect( screenshotAct, SIGNAL( triggered() ), this, SLOT( screenshot() ) );
    fileMenu->addAction( screenshotAct );
    screenshotAct->setWhatsThis( tr( "The function offers you to save a screenshot in a png file. Unfortunately the outer frame of the main window is not saved, only the application itself." ) );

    fileMenu->addSeparator();

    QAction* quitAct = new QAction( tr( "&Quit" ), this );
    quitAct->setShortcut( tr( "Ctrl+Q" ) );
    quitAct->setStatusTip( tr( "Exits the application" ) );
    connect( quitAct, SIGNAL( triggered() ), this, SLOT( closeApplication() ) );
    fileMenu->addAction( quitAct );
    quitAct->setWhatsThis( tr( "Closes the application." ) );

    fileMenu->addSeparator();

    // display menu

    QMenu* displayMenu = menuBar()->addMenu( tr( "&Display" ) );
    displayMenu->setStatusTip( tr( "Ready" ) );

    splitterOrderAct = new QAction( tr( "D&imension order..." ), this );
    splitterOrderAct->setStatusTip( tr( "Sets the order of the dimensions metric, call chain, and system." ) );
    connect( splitterOrderAct, SIGNAL( triggered() ), this, SLOT( setDimensionOrder() ) );
    displayMenu->addAction( splitterOrderAct );
    splitterOrderAct->setWhatsThis( tr( "As explained above, Cube has three resizable panes. Initially the metric pane is on the left, the call pane is in the middle, and the system pane is on the right-hand-side. However, sometimes you may be interested in other orders, and that is what this menu item is about. It offers all possible pane orderings.  For example, assume you would like to see the metric and call values for a certain thread.  In this case, you should place the system pane on the left, the metric pane in the middle, and the call pane on the right. Note that in panes left-hand-side of the metric pane we have no meaningful values, since they miss a reference metric; in this case we specify the values to be undefined, denoted by a \"-\" sign." ) );

    displayMenu->addSeparator();
    mapMenu = displayMenu->addMenu( tr( "Choose colormap" ) );
    mapMenu->setStatusTip( tr( "Choose a colormap" ) );
    connect( mapMenu, SIGNAL( aboutToShow() ), this, SLOT( updateColormapMenu() ) );
    mapMenu->setEnabled( false );

    colorsAct = new QAction( tr( "Edit colormap..." ), this );
    colorsAct->setStatusTip( tr( "Edit the selected colormap" ) );
    connect( colorsAct, SIGNAL( triggered() ), this, SLOT( editColorMap() ) );
    displayMenu->addAction( colorsAct );
    colorsAct->setWhatsThis( tr( "Opens a dialog that allows the user to edit the selected colormap" ) );
    colorsAct->setEnabled( false );
    displayMenu->addSeparator();

    QAction* styleSheetAct = new QAction( tr( "Customize style sheets..." ), this );
    connect( styleSheetAct, SIGNAL( triggered() ), StyleSheetEditor::getInstance(), SLOT( configureStyleSheet() ) );
    displayMenu->addAction( styleSheetAct );
    styleSheetAct->setWhatsThis( tr( "Opens a dialog that allows the user to configure the appearance of the controls" ) );

    valueViewAct = new QAction( tr( "Configure value view..." ), this );
    connect( valueViewAct, SIGNAL( triggered() ), this, SLOT( configureValueView() ) );
    displayMenu->addAction( valueViewAct );
    valueViewAct->setWhatsThis( tr( "Opens a dialog that allows the user to configure the value view of the tree items" ) );
    valueViewAct->setEnabled( false );

    QAction* precisionAct = new QAction( tr( "&Precision..." ), this );
    precisionAct->setStatusTip( tr( "Defines the precision of the display for numbers." ) );
    connect( precisionAct, SIGNAL( triggered() ), this, SLOT( setPrecision() ) );
    precisionAct->setWhatsThis( tr( "Activating this menu item opens a dialog for precision settings. See also help on the dialog itself." ) );
    displayMenu->addAction( precisionAct );

    treeMenu = TreeConfig::getInstance()->getMenu();
    displayMenu->addMenu( treeMenu );

    QMenu* widthMenu = displayMenu->addMenu( tr( "Optimize width" ) );
    widthMenu->setStatusTip( tr( "Ready" ) );
    widthMenu->setWhatsThis( tr( "Under this menu item Cube offers widget rescaling such that the amount of information shown is maximized, i.e., Cube optimally distributes the available space between its components. You can chose if you would like to stick to the current main window size, or if you allow to resize it." ) );

    QAction* width1Act = new QAction( tr( "&Keep main window size" ), this );
    width1Act->setStatusTip( tr( "Optimize widths of tabs in order to show equally large percentages." ) );
    connect( width1Act, SIGNAL( triggered() ), this, SLOT( distributeWidth() ) );
    width1Act->setWhatsThis( tr( "Under this menu item Cube offers widget rescaling with keeping the main window size, such that the amount of information shown is maximized, i.e., Cube optimally distributes the available space between its components." ) );
    widthMenu->addAction( width1Act );
    displayMenu->addSeparator();

    QAction* width2Act = new QAction( tr( "&Adapt main window size" ), this );
    width2Act->setStatusTip( tr( "Resizes the main window width and adapt tab widths in order to show all information" ) );
    connect( width2Act, SIGNAL( triggered() ), this, SLOT( adaptWidth() ) );
    width2Act->setWhatsThis( tr( "Under this menu item Cube offers widget rescaling possibly changing the main window size, such that the amount of information shown is maximized, i.e., Cube optimally distributes the available space between its components." ) );
    widthMenu->addAction( width2Act );

    syncAction = new QAction( tr( "Show synchronization toolbar" ), this );
    syncAction->setCheckable( true );
    connect( syncAction, SIGNAL( toggled( bool ) ), syncToolBar, SLOT( setVisible( bool ) ) );
    displayMenu->addAction( syncAction );

    displayMenu->addAction( settings->getBookmarkToolbarAction() );

    presentationAction = new QAction( tr( "Enable presentation mode" ), this );
    presentationAction->setCheckable( true );
    connect( presentationAction, SIGNAL( toggled( bool ) ), this, SLOT( setPresentationMode( bool ) ) );
    displayMenu->addAction( presentationAction );

    QMenu* pluginMenu = PluginManager::getInstance()->getPluginMenu();
    menuBar()->addMenu( pluginMenu );

    // help menu

    QMenu* helpMenu = menuBar()->addMenu( tr( "&Help" ) );
    helpMenu->setStatusTip( tr( "Ready" ) );

    QAction* introAct = new QAction( tr( "Getting started" ), this );
    introAct->setStatusTip( tr( "Displayes a short introduction to Cube." ) );
    connect( introAct, SIGNAL( triggered() ), this, SLOT( introduction() ) );
    helpMenu->setWhatsThis( tr( "Opens a dialog with some basic information on the usage of Cube." ) );
    helpMenu->addAction( introAct );

    QAction* manual = new QAction( tr( "User guide" ), this );
    manual->setStatusTip( tr( "Shows Cube user guide" ) );
    connect( manual, SIGNAL( triggered() ), this, SLOT( showGuide() ) );
    manual->setWhatsThis( tr( "Shows Cube GUI user guide in the help browser." ) );
    helpMenu->addAction( manual );

    QAction* keysAct = new QAction( tr( "Mouse and keyboard control" ), this );
    keysAct->setStatusTip( tr( "Shows all supported mouse and keyboard controls" ) );
    connect( keysAct, SIGNAL( triggered() ), this, SLOT( keyHelp() ) );
    keysAct->setWhatsThis( tr( "List all control possibilities for keyboard and mouse." ) );
    helpMenu->addAction( keysAct );

    QAction* whatsThisAct = QWhatsThis::createAction();
    whatsThisAct->setStatusTip( tr( "Change into help mode for display components" ) );
    whatsThisAct->setWhatsThis( tr( "Here you can get more specific information on parts of the Cube GUI. If you activate this menu item, you switch to the \"What's this?\" mode. If you now click on a widget an appropriate help text is shown. The mode is left when help is given or when you press Esc.\n\nAnother way to ask the question is to move the focus to the relevant widget and press Shift+F1." ) );
    helpMenu->addAction( whatsThisAct );

    QAction* aboutAct = new QAction( tr( "About" ), this );
    aboutAct->setStatusTip( tr( "Shows Cube's about box" ) );
    connect( aboutAct, SIGNAL( triggered() ), this, SLOT( about() ) );
    aboutAct->setWhatsThis( tr( "Opens a dialog with some release information." ) );
    helpMenu->addAction( aboutAct );

    helpMenu->addSeparator();

    helpMenu->addMenu( PluginManager::getInstance()->getPluginInfoMenu() );
    helpMenu->addMenu( PluginManager::getInstance()->getPluginHelpMenu() );
    helpMenu->addSeparator();

    performanceInfoAct = new QAction( tr( "Selected metrics description" ), this );
    performanceInfoAct->setStatusTip( tr( "Shows the online description of the selected metrics." ) );
    performanceInfoAct->setWhatsThis( tr( "Shows some (usually more extensive) online description for the selected metrics. For metrics it might point to an online documentation explaining their semantics." ) );
    helpMenu->addAction( performanceInfoAct );
    performanceInfoAct->setEnabled( false );

    regionInfoAct = new QAction( tr( "Selected regions description" ), this );
    regionInfoAct->setStatusTip( tr( "Shows the online description of the selected regions in a  program." ) );
    regionInfoAct->setWhatsThis( tr( "Shows some (usually more extensive) online description for the clicked region in program. For regions representing library functions it might point to the corresponding library documentation." ) );
    helpMenu->addAction( regionInfoAct );
    regionInfoAct->setEnabled( false );
}


void
MainWidget::introduction()
{
    QString url = Globals::getOption( DocPath ) + "cubegui/guide/html/userguide.html#intro";
    HelpBrowser::getInstance()->showUrl( url, tr( "Cube User Guide is not installed" ) );
}

void
MainWidget::keyHelp()
{
    QString url = Globals::getOption( DocPath ) + "cubegui/guide/html/keyboardcontrol.html";
    HelpBrowser::getInstance()->showUrl( url, tr( "Cube User Guide is not installed" ) );
}

void
MainWidget::showGuide()
{
    QString url = Globals::getOption( DocPath ) + "cubegui/guide/html/index.html";
    HelpBrowser::getInstance()->showUrl( url, tr( "Cube User Guide is not installed" ) );
}

/*************************************************************/
/************ widget title, status bar ***********************/
/*************************************************************/

// update the title of the application to show infos to the loaded file and external file
//
void
MainWidget::updateWidgetTitle()
{
    // CUBE_NAME is defined in constants.h
    QString widgetTitle( CUBEGUI_FULL_NAME );

    // append the name of the currently opened file
    if ( cube != NULL && fileLoaded )
    {
        QUrl      fileUrl       = QUrl( cube->getAttribute( "cubename" ).c_str() );
        QFileInfo openFileName  = QFileInfo( fileUrl.path() );
        QString   fileName      = openFileName.fileName();
        QString   lastDirectory = openFileName.dir().dirName();

        widgetTitle.append( ": " );
        widgetTitle.append( lastDirectory );
        widgetTitle.append( QDir::separator() );
        widgetTitle.append( fileName );
    }

    // append the name of the current external file
    QString externalName = lastExternalFileName;
    if ( externalName != "" )
    {
        QFileInfo externalFile  =  QFileInfo( externalName );
        QString   fileName      = externalFile.fileName();
        QString   lastDirectory = externalFile.dir().dirName();

        widgetTitle.append( tr( " external: " ) );
        widgetTitle.append( lastDirectory );
        widgetTitle.append( QDir::separator() );
        widgetTitle.append( fileName );
    }

    setWindowTitle( widgetTitle );
}

// sets the status bar text
//
void
MainWidget::setMessage( const QString& message, MessageType type, bool isLogged )
{
    statusBar->addLine( message, type, isLogged );
    statusBar->repaint(); // use repaint to show current status immediately
}


/*************************************************************/
/******************** file handling **************************/
/*************************************************************/


// remember the names of the last FILE_HISTORY_COUNT files that were opened;
// they can be re-opened via the display menu
//
void
MainWidget::rememberFileName( QString fileName )
{
    cube::Url fileUrl( fileName.toStdString() );
    QString   savedName = fileName;

    if ( fileUrl.getProtocol() == "file" )
    {
        savedName = QString( fileUrl.getPath().c_str() );
    }

#ifdef obsolete
    // Everything but 'file' is handled as generic URL.
    if ( fileUrl.getProtocol() != "file" )
    {
        openedUrls.prepend( fileName );
        openedUrls.removeDuplicates();

        return;
    }
#endif

    // clear the QActions from the display menu to re-open files
    for ( int i = 0; i < ( int )lastFileAct.size(); i++ )
    {
        fileMenu->removeAction( lastFileAct.at( i ) );
    }
    lastFileAct.clear();

    // insert the new fileName at the beginning of the openedFiles vector
    // and remove eventual duplicates
    openedFiles.push_back( QString( "" ) );
    for ( int i = openedFiles.size() - 1; i > 0; i-- )
    {
        openedFiles[ i ] = openedFiles[ i - 1 ];
    }
    openedFiles[ 0 ] = savedName;
    for ( int i = 1; i < ( int )openedFiles.size(); i++ )
    {
        if ( openedFiles[ i ].compare( openedFiles[ 0 ] ) == 0 )
        {
            for ( int j = i; j < ( int )openedFiles.size() - 1; j++ )
            {
                openedFiles[ j ] = openedFiles[ j + 1 ];
            }
            openedFiles.pop_back();
            break;
        }
    }

    // we remember at most FILE_HISTORY_COUNT file names
    if ( openedFiles.size() > FILE_HISTORY_COUNT )
    {
        openedFiles.pop_back();
    }

    for ( int i = 0; i < openedFiles.size(); i++ )
    {
        QString   filename = openedFiles.at( i );
        QFileInfo file( filename );
        QString   lastDir     = file.absolutePath().split( '/' ).last();
        QString   displayName = lastDir + QDir::separator() + file.fileName();

        cube::Url fileUrl( filename.toStdString() );
        if ( fileUrl.getProtocol() == "cube" )
        {
            displayName = QString( fileUrl.getProtocol().c_str() ) + "://.../" + displayName;
        }

        QAction* act = new QAction( displayName, this );
        if ( i < 5 )
        {
            fileMenu->addAction( act );
        }
        act->setData( filename );
        connect( act, SIGNAL( triggered() ), this, SLOT( openLastFiles() ) );

        act->setStatusTip( filename );
        act->setWhatsThis( tr( "At the bottom of the File menu the last opened files are offered for re-opening, the top-most being the most recently opened one. A full path to the file is visible in the status bar if you move the mouse above one of the recent file items in the menu." ) );
        lastFileAct.push_back( act );
    }

    if ( recentFileWidget ) // update initial screen contents and size
    {
        QWidget*     widget = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout();
        layout->setSpacing( 0 );
        widget->setLayout( layout );

        for ( int i = 0; i < ( int )lastFileAct.size(); i++ )
        {
            QFileInfo file( openedFiles[ i ] );
            QString   lastDir = file.absolutePath().split( '/' ).last();
            QString   name    = lastDir + QDir::separator() + file.fileName();

            cube::Url fileUrl( openedFiles[ i ].toStdString() );
            if ( fileUrl.getProtocol() == "cube" )
            {
                name = QString( fileUrl.getProtocol().c_str() ) + "://.../" + name;
            }

            HoverLabel* label = new HoverLabel( "<a href=" + openedFiles[ i ] + ">" + name + "</a>", i );
            label->setContentsMargins( 3, 3, 3, 3 );
            layout->addWidget( label );
            connect( label, SIGNAL( linkActivated( QString ) ), this, SLOT( recentFileSelected( QString ) ) );
            connect( label, SIGNAL( hovered( int ) ), this, SLOT( recentFileHovered( int ) ) );
            connect( label, SIGNAL( leave() ), this, SLOT( recentFileHovered() ) );
        }

        recentFileWidget->setWidget( widget );
    }
}

// end of rememberFileName(...)

// private slot connected to a file menu item which shows one of the recently opened files
void
MainWidget::openLastFiles()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if ( action != 0 )
    {
        QString filename = action->data().toString();
        loadFile( filename );
    }
}

// input a file name to open and open it via loadFile(...)
//
void
MainWidget::saveAs()
{
    QString saveAsFileName = QFileDialog::getSaveFileName( this,
                                                           tr( "Choose a file to open" ),
                                                           "",
                                                           tr( "Cube4 files (*.cubex);;All files (*.*);;All files (*)" ) );

    if ( saveAsFileName.length() == 0 )
    {
        setMessage( tr( "Ready" ) );
        return;
    }
    // here is the saving name selecetd

    setMessage( tr( "Saving " ).append( saveAsFileName ).append( "..." ) );
    setCursor( Qt::WaitCursor );
    writeCube( saveAsFileName );

    unsetCursor();
    setMessage( tr( "Ready" ) );
}



// input a file name to open and open it via loadFile(...)
//
void
MainWidget::openFile()
{
    QString lastName     = ( openedFiles.size() == 0 ? "." : openedFiles[ 0 ] );
    QString openFileName = QFileDialog::getOpenFileName( this,
                                                         tr( "Choose a file to open" ),
                                                         lastName,
                                                         "Cube3/4 files (*cube *cube.gz *.cubex);;Cube4 files (*.cubex);;Cube3 files (*.cube.gz *.cube);;All files (*.*);;All files (*)" );

    if ( openFileName.length() == 0 )
    {
        setMessage( tr( "Ready" ) );
        return;
    }
    setMessage( tr( "Open " ).append( openFileName ).append( "..." ) );
    loadFile( openFileName );
}

//input a URL to open a remote file and... //TODO
//
void
MainWidget::openRemote()
{
    QString openRemoteName = RemoteFileDialog::getFileName();

    if ( !openRemoteName.isEmpty() )
    {
        setMessage( tr( "Open " ).append( openRemoteName ).append( "..." ) );

        loadFile( openRemoteName );
    }
}

// load the contents of a cube file into a cube object
// and display the contents
//
bool
MainWidget::loadFile( const QString fileName )
{
    qDebug() << tr( "open file " ) << fileName;

    setMessage( tr( "Closing previous Cube report..." ) );
    closeFile();

    fileLoaded = false;
    setMessage( tr( "Opened " ) + fileName + tr( ". Parsing..." ) );
    setCursor( Qt::WaitCursor );
    stackedWidget->setCurrentIndex( CONTEXT_EMPTY );

    assert( cube == NULL );

    timer = new QTimer();
    timer->start( 300 );
    connect( timer, SIGNAL( timeout() ), this, SLOT( updateProgress() ) );

    // create proxy
    try
    {
        cube::Url fileUrl( fileName.toStdString() );
        cube = cube::CubeProxy::create( fileUrl.toString() );
        ReadCubeThread* w = new ReadCubeThread( cube );
        connect( w, SIGNAL( cubeLoaded( QString ) ), this, SLOT( cubeReportLoaded( QString ) ) );

        // calls openCubeReport in separate thread
        w->start();
    }
    catch ( const std::exception& err )
    {
        cubeReportLoaded( err.what() );
        unsetCursor();
        setMessage( QString( err.what() ), Critical );
    }

    return true;
}

void
MainWidget::updateProgress()
{
    if ( fileLoaded || cube == NULL )
    {
        return;
    }
    const cube::ProgressStatus& status  = cube->getOperationProgressStatus();
    int                         percent = ( int )( status.status * 100 + .5 );
    QString                     message = QString( status.message.c_str() );

    message += " (" + QString::number( percent ) + "%)";
    //qDebug() << "current progress " << percent << message;
    Globals::setStatusMessage( message, Information, false );
}

void
MainWidget::writeCube( const QString& filename ) const
{
    CubeWriter( cube,
                tabManager->getTree( METRICTREE ),
                tabManager->getTree( DEFAULTCALLTREE ),
                tabManager->getTree( TASKTREE ),
                tabManager->getTree( SYSTEMTREE ) ).writeCube( filename );
}

void
MainWidget::cubeReportLoaded( const QString& errorMessage )
{
    timer->stop();
    timer->deleteLater();
    unsetCursor();
    if ( !errorMessage.isEmpty() )
    {
        fileLoaded = false;
        setMessage( tr( "Error during opening cube file: " ) + QString( errorMessage ), Error );
        delete cube;
        cube = NULL;
        stackedWidget->setCurrentIndex( CONTEXT_INIT );
    }
    else
    {
        fileLoaded = true;
        setMessage( tr( "Finished parsing..." ) );
        setMessage( tr( "Creating GUI..." ) );
        try
        {
            openCube( cube, cube->getUrl().toString().c_str() );
            // remember the file name for opening recent files
            rememberFileName( cube->getUrl().toString().c_str() );

            updateWidgetTitle();
            stackedWidget->setCurrentIndex( CONTEXT_CUBE );
            if ( !initialSystemTab.isNull() )
            {
                Globals::getTabManager()->getTab( SYSTEM )->toFront( initialSystemTab );
            }
        }
        catch ( const exception& e )
        {
            setMessage( tr( "Error opening cube file: " ) + QString( e.what() ), Error );
            delete cube;
            cube = NULL;
            stackedWidget->setCurrentIndex( CONTEXT_INIT );
        }
    }
}

void
MainWidget::openCube( cube::CubeProxy* cube, const QString& fileName )
{
    setMessage( tr( "Creating display..." ) );
    stackedWidget->setCurrentIndex( CONTEXT_EMPTY );

    PluginManager::getInstance()->closeContextFreePlugin();

    this->cube = cube;

    // initialize the tree views in their display order
    setMessage( tr( "Creating trees..." ) );

    tabManager->cubeOpened( cube );
    tabManager->show();
    foreach( TabWidget * widget, tabManager->getTabWidgets() )
    {
        connect( widget, SIGNAL( externalValueModusSelected( TabWidget* ) ),
                 this, SLOT( selectExternalValueMode( TabWidget* ) ) );
    }

    setMessage( tr( "Computing values..." ) );

    emit enableClose( true );

    setMessage( tr( "Opening plugins..." ) );
    // open all plugins, in a final step all plugin tabs will be added
    QApplication::setOverrideCursor( Qt::WaitCursor );
    PluginManager::getInstance()->opened( cube, fileName, tabManager );
    QApplication::restoreOverrideCursor();
    setMessage( tr( "Opening plugins done" ), Information, false );

    setMessage( tr( "Loading Settings..." ) );
    // load experiment specific settings
    setUpdatesEnabled( false );
    settings->cubeLoaded( fileName );
    setUpdatesEnabled( true );

    setMessage( tr( "Calculating tree values..." ) );

    stackedWidget->setCurrentIndex( CONTEXT_CUBE );

    // activate loaded experiment specific settings or set initial status
    setMessage( tr( "Initializing menu..." ) );
    tabManager->initialize();
    // enable menu items
    colorsAct->setEnabled( true );
    treeMenu->setEnabled( true );
    mapMenu->setEnabled( true );
    colorScale->setVisible( true );
    saveAsAct->setEnabled( true );
    performanceInfoAct->setEnabled( true );
    regionInfoAct->setEnabled( true );
    valueViewAct->setEnabled( true );
    openExtAct->setEnabled( true );

    // connect help menu items to slots of the current views
    connect( performanceInfoAct, SIGNAL( triggered() ),
             tabManager->getActiveView( METRIC ), SLOT( onShowDocumentation() ) );
    connect( regionInfoAct, SIGNAL( triggered() ), this, SLOT( onRegionInfo() ) );

    // register for synchronization
    QList<SettingsHandler*> handlerList;

    // notify the synchronization toolbar about all changes in the tree view
    foreach( TreeView * view, Globals::getTabManager()->getViews() )
    {
        connect( view, SIGNAL( selectionChanged( Tree* ) ), syncToolBar, SLOT( send() ) );
        connect( view, SIGNAL( expanded( QModelIndex ) ), syncToolBar, SLOT( send() ) );
        connect( view, SIGNAL( collapsed( QModelIndex ) ), syncToolBar, SLOT( send() ) );
        handlerList.append( view );
    }
    syncToolBar->setSettingsHandlerList( handlerList );

    stackedWidget->setCurrentIndex( CONTEXT_CUBE );
}

// end of loadFile(...)


// remove loaded data
//
void
MainWidget::closeFile()
{
    if ( cube == 0 )
    {
        return;              // already closed
    }

    fileLoaded = false;
    syncToolBar->clearSettingsHandler();

    settings->cubeClosed();
    lastColorMapName = Globals::getColorMap()->getMapName();

    PluginManager::getInstance()->closed();

    tabManager->cubeClosed();

    delete cube;
    cube = NULL;
    updateWidgetTitle();

    emit enableClose( false );
    emit cubeIsClosed();
    setMessage( tr( "Ready" ) );

    // disable menu items
    saveAsAct->setEnabled( false );
    colorsAct->setEnabled( false ); // disable colormap selection
    colorScale->setVisible( false );
    treeMenu->setEnabled( false );
    mapMenu->setEnabled( false );
    performanceInfoAct->setEnabled( false );
    regionInfoAct->setEnabled( false );
    valueViewAct->setEnabled( false );
    openExtAct->setEnabled( false );

    performanceInfoAct->disconnect(); // disconnect all slots
    regionInfoAct->disconnect();

    stackedWidget->setCurrentIndex( CONTEXT_INIT );
}

/**
 * This method is called, if the user selects EXTERNAL value modus. If no external cube file is loaded,
 * a file dialog is opened.
 */
void
MainWidget::selectExternalValueMode( TabWidget* widget )
{
    bool ok = false;
    if ( lastExternalFileName.size() > 0 )
    {
        ok = true;
    }
    else
    {
        ok = openExternalFile();
    }
    if ( ok )
    {
        widget->setValueModus( EXTERNAL_VALUES );
    }
    else
    {
        widget->setValueModus( ABSOLUTE_VALUES );
    }
}

// input a file name to open as external file for external percentage
// and open it via readExternalFile(...)
//
bool
MainWidget::openExternalFile()
{
    QString fileName = QFileDialog::getOpenFileName( this,
                                                     tr( "Choose a file for external percentage" ),
                                                     lastExternalFileName,
                                                     "Cube3/4 files (*cube *cube.gz *.cubex);;Cube4 files (*.cubex);;Cube3 files (*.cube.gz *.cube);;All files (*.*);;All files (*)" );

    if ( fileName.length() == 0 )
    {
        setMessage( tr( "Ready" ) );
        return false;
    }
    bool result = readExternalFile( fileName );
    if ( result )
    {
        updateWidgetTitle();
        emit enableExtClose( true );
    }

    return result;
}


// read the contents of a cube file into a temporary cube object for
// the external percentage modus and compute the relevant values for the display
//
bool
MainWidget::readExternalFile( const QString fileName )
{
    setMessage( tr( "Parsing..." ) );
    setCursor( Qt::WaitCursor );

    // #ifndef CUBE_COMPRESSED
    //     std::ifstream in( fileName.toStdString().c_str() );
    // #else
    //     gzifstream    in( fileName.toStdString().c_str(), ios_base::in | ios_base::binary );
    // #endif

    lastExternalFileName = fileName;

    // read in external data into the external cube object
    assert( cubeExternal == NULL );
    cubeExternal = new cube::CubeIoProxy();
    try
    {
        cubeExternal->openReport( fileName.toStdString() );
        cubeExternal->setMemoryStrategy( strategy );
        // compute external reference values for metric tree
        MetricTree* metricTree = static_cast<MetricTree*> ( tabManager->getActiveTree( METRIC ) );
        metricTree->computeExternalReferenceValues( cubeExternal );
    }
    catch ( const std::exception& e )
    {
        setMessage( e.what(), Error );
    }

    // the relevant values got stores in the metric tab widget
    // and we can delete the external cube object
    delete cubeExternal;
    cubeExternal = NULL;

    unsetCursor();
    setMessage( tr( "Ready" ) );

    return true;
}

// remove loaded external data for external percentage
//
void
MainWidget::closeExternalFile()
{
    // check if external modus is set in one of the tabs;

    foreach( TabWidget * widget, tabManager->getTabWidgets() )
    {
        if ( widget->getValueModus() == EXTERNAL_VALUES )
        {
            widget->setValueModus( ABSOLUTE_VALUES );
        }
    }

    lastExternalFileName = "";
    updateWidgetTitle();
    emit enableExtClose( false );
}


/*********************************************************/
/****************** miscellaneous ************************/
/*********************************************************/

void
MainWidget::closeApplication()
{
    close(); // closeEvent is called
}

// clean-up before close
// closes the application after eventually saving settings


// for calling scorep_finalize(), should be function-name in C++ have 'C' linkage : extern "C" void scorep_finalize(void);
DEFINE_SCOREP_FINALIZE();

void
MainWidget::closeEvent( QCloseEvent* event )
{
    // Before closing the window by choosing "Close" from the menu, or clicking the X button, scorep_finalize() should be called.
    //CALL_SCOREP_FINALIZE();

    Q_UNUSED( event );

    closeFile();
    qApp->quit();
}

// set the order of metric/call/system splitter elements
//
void
MainWidget::setDimensionOrder()
{
    DimensionOrderDialog dialog( this, tabManager->getOrder() );
    if ( dialog.exec() )
    {
        QList<DisplayType> order = dialog.getOrder();

        tabManager->setOrder( order );
    }
}

void
MainWidget::updateColormapMenu()
{
    mapMenu->clear();

    QList<ColorMap*> maps = PluginManager::getInstance()->getColorMaps();
    maps.prepend( Globals::defaultColorMap );

    QActionGroup* selectionActionGroup = new QActionGroup( this );
    selectionActionGroup->setExclusive( true );
    QString currentMapName = Globals::getColorMap()->getMapName();
    foreach( ColorMap * map, maps )
    {
        QAction* action = mapMenu->addAction( map->getMapName() );
        action->setCheckable( true );
        selectionActionGroup->addAction( action );
        action->setData( QVariant::fromValue( ( void* )map ) );
        connect( action, SIGNAL( triggered() ), this, SLOT( selectColorMap() ) );
        if ( map->getMapName() == currentMapName )
        {
            action->setChecked( true );
        }
    }
}

// causes to open a color dialog to allow the user to change the colormap
void
MainWidget::editColorMap()
{
    ColorMap* map = Globals::getColorMap();
    map->showDialog();
}

// slot for color map menu item
void
MainWidget::selectColorMap()
{
    QAction*  action = qobject_cast<QAction*>( sender() );
    ColorMap* map    = static_cast<ColorMap*> ( action->data().value<void*>() );
    setColorMap( map );
}

void
MainWidget::setColorMap( ColorMap* map )
{
    ColorMap* old = Globals::getColorMap();
    disconnect( old, SIGNAL( colorMapChanged() ), this, SLOT( updateColorMap() ) );
    connect( map, SIGNAL( colorMapChanged() ), this, SLOT( updateColorMap() ) );
    // remember current map name to store in settings
    lastColorMapName = map->getMapName();

    if ( old != map )
    {
        updateColorMap( map );
    }
}

// slot, called if apply is pressed in edit dialog
void
MainWidget::updateColorMap( ColorMap* map )
{
    if ( map == 0 )
    {
        map = Globals::getColorMap();
    }
    Globals::setColorMap( map );
    if ( cube ) // notify plugin tabs
    {
        tabManager->updateColors();
    }
}

void
MainWidget::configureValueView()
{
    QSet<cube::DataType> usedTypes;

    Tree*                  tree     = this->tabManager->getActiveTree( METRIC );
    const QList<TreeItem*> toplevel = tree->getRootItem()->getChildren();
    foreach( TreeItem * topItem, tree->getRootItem()->getChildren() )
    {
        usedTypes.insert( topItem->getValueObject()->myDataType() );
    }

    cube::DataType   type   = tree->getLastSelection()->getValueObject()->myDataType();
    ValueViewConfig* config = new ValueViewConfig( usedTypes.values(), type, this );
    config->setVisible( true );
    connect( this, SIGNAL( cubeIsClosed() ), config, SLOT( close() ) );
}



// causes to open a precision dialog to allow the user to change precision-specific settings
//
void
MainWidget::setPrecision()
{
    PrecisionWidget* precisionWidget = Globals::getPrecisionWidget();
    precisionWidget->exec();
    setMessage( tr( "Ready" ) );
}

void
MainWidget::onRegionInfo()
{
    assert( cube != NULL );
    TreeView* view = tabManager->getActiveView( CALL );
    view->onShowDocumentation();
}

// shows the about message
//
void
MainWidget::about()
{
    QDialog dialog( this );
    dialog.setWindowIcon( QIcon( ":images/CubeIcon.xpm" ) );
    dialog.setWindowTitle( QObject::tr( "About " ).append( CUBEGUI_FULL_NAME ) );

    QLabel* cube_title    = new QLabel( tr( "<b>This is " ).append( CUBEGUI_FULL_NAME " </b> (rev. <b>" CUBEGUI_REVISION "</b> ) (c) 2009-2021" ) );
    QLabel* qt_version    = new QLabel( tr( "Built with <b>Qt v" ).append( QT_VERSION_CUBE " </b>" ) );
    QLabel* qmake         = new QLabel( tr( "Built with qmake: <b>" ).append( QT_PATH "</b>" ) );
    QLabel* cube_homepage = new QLabel( tr( "Home page" ).append( "        : <a href=\"http://www.scalasca.org\"> " PACKAGE_URL " </a>" ) );
    cube_homepage->setOpenExternalLinks( true );
    QLabel* cube_support = new QLabel( tr( "Technical support: " ).append( " <a href=\"mailto:scalasca@fz-juelich.de?subject=" CUBEGUI_FULL_NAME ).append( tr( " Feedback.&body=Dear Scalasca Team, \n\nwe would like to inform you, that " )  ).append( CUBEGUI_FULL_NAME " (rev. " CUBEGUI_REVISION ")" ).append( tr( "is awesome and we would like to have additional feature: a Teleport and an X-Wing fighter support. \n\n\n\n\n Sincerely,\n User of " ) ).append( CUBEGUI_FULL_NAME "\">" PACKAGE_BUGREPORT "</a>" ) );
    cube_support->setOpenExternalLinks( true );


    QLabel* fz_copyright = new QLabel( tr( "Juelich Supercomputing Centre,\nForschungszentrum Juelich GmbH" ) );

    QLabel* fz_logo = new QLabel();
    fz_logo->setPixmap( QPixmap( ":/images/fzjlogo.xpm" ).scaledToWidth( 200 / 1.3,  Qt::SmoothTransformation ) );
    QLabel* cube_logo = new QLabel();
    cube_logo->setPixmap( QPixmap( ":/images/CubeLogo.xpm" ).scaledToWidth( 200 / 1.3,  Qt::SmoothTransformation ) );

    QVBoxLayout* main_layout        = new QVBoxLayout();
    QHBoxLayout* fz_layout          = new QHBoxLayout();
    QHBoxLayout* cube_layout        = new QHBoxLayout();
    QVBoxLayout* fz_labels_layout   = new QVBoxLayout();
    QVBoxLayout* cube_labels_layout = new QVBoxLayout();
    QVBoxLayout* fz_logos_layout    = new QVBoxLayout();
    QVBoxLayout* cube_logos_layout  = new QVBoxLayout();

    main_layout->addLayout( cube_layout );
    main_layout->addLayout( fz_layout );

    fz_layout->addLayout( fz_logos_layout );
    fz_layout->addLayout( fz_labels_layout );

    cube_layout->addLayout( cube_logos_layout );
    cube_layout->addLayout( cube_labels_layout );


    main_layout->setAlignment( Qt::AlignHCenter );

    fz_layout->setAlignment( Qt::AlignLeading );
    fz_labels_layout->setAlignment( Qt::AlignLeading );

    cube_labels_layout->setAlignment( Qt::AlignLeading );
    cube_labels_layout->setAlignment( cube_title,  Qt::AlignHCenter );
    cube_labels_layout->setAlignment( qt_version,  Qt::AlignHCenter );
    cube_labels_layout->setAlignment( qmake,  Qt::AlignHCenter );

    /*  QVBoxLayout * layout = new QVBoxLayout();
       QVBoxLayout * logos_layout = new QVBoxLayout();*/

    main_layout->setSpacing( 5 );
    cube_layout->setSpacing( 5 );
    fz_layout->setSpacing( 5 );
    cube_labels_layout->setSpacing( 1 );

    fz_labels_layout->addWidget( fz_copyright );
    fz_logos_layout->addWidget( fz_logo );

    cube_labels_layout->addWidget( cube_title );
    cube_labels_layout->addWidget( cube_homepage );
    cube_labels_layout->addWidget( cube_support );
    cube_labels_layout->addWidget( qt_version );
    cube_labels_layout->addWidget( qmake );

    cube_logos_layout->addWidget( cube_logo );

    dialog.setLayout( main_layout );

    QDialogButtonBox* buttonBox = new QDialogButtonBox( &dialog );
    buttonBox->setStandardButtons( QDialogButtonBox::Close );
    connect( buttonBox, SIGNAL( clicked( QAbstractButton* ) ), &dialog, SLOT( accept() ) );
    main_layout->addWidget( buttonBox );

    dialog.exec();

    setMessage( tr( "Ready" ) );
}

// sets the file size threshold above which dynamic loading of metric data is enabled
//
void
MainWidget::setDynloadThreshold()
{
    bool ok;
    int  value = QInputDialog::getInt( this, tr( "Dynamic loading threshold" ),
                                       tr( "File size threshold (byte) above which files are dynamically loaded: " ),
                                       dynloadThreshold, 0, 1000000000, 50000000, &ok );
    if ( ok )
    {
        dynloadThreshold = value;
    }
}

void
MainWidget::setPresentationMode( bool enable )
{
    if ( presentationAction->isChecked() != enable ) // slot has not been activated by the action -> check action to update menu item
    {
        presentationAction->setChecked( enable );
    }
    else
    {
        app.setPresentationMode( enable );
    }
}

// saves a screenshot in a png file
// (it is without the shell frame, I could not find any possibilities to include that frame within qt)
//
void
MainWidget::screenshot()
{
    QPixmap pixmap;
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
    pixmap = this->grab();
#else
    pixmap = QPixmap::grabWidget( this );
#endif

    QImage image = pixmap.toImage();

    QString format      = "png";
    QString initialPath = QDir::currentPath() + tr( "/cube." ) + format;

    QString fileName = QFileDialog::getSaveFileName( this, tr( "Save As" ),
                                                     initialPath,
                                                     tr( "%1 Files (*.%2);;All Files (*)" )
                                                     .arg( format.toUpper() )
                                                     .arg( format ) );
    if ( !fileName.isEmpty() )
    {
        image.save( fileName, format.toLocal8Bit() );
    }

    setMessage( tr( "Ready" ) );
}

/** distribute the available width between the 3 tabs such that from each scroll area the same percentual
 * amount is visible
 */
void
MainWidget::distributeWidth()
{
    // get the current sizes of the splitter's widgets
    QList<int> sizes   = tabManager->sizes();
    int        current = 0;
    foreach( int val, sizes )
    {
        current += val;
    }

    // get optimal widths for the splitter's widgets
    int preferred = 0;
    int idx       = 0;
    foreach( TabWidget * widget, tabManager->getTabWidgets() )
    {
        sizes[ idx ] = widget->sizeHint().width();
        preferred   += sizes[ idx++ ];
    }
    // distribute the available width
    for ( int i = 0; i < 3; i++ )
    {
        sizes[ i ] = ( int )( ( double )current * ( double )sizes[ i ] / ( double )preferred );
    }
    // resize
    tabManager->setSizes( sizes );
}

/** adapt the size of the main window such that all information are visible and resize the splitter widget
 *  accordingly */
void
MainWidget::adaptWidth()
{
    // get optimal widths for the splitter's widgets
    QList<int> sizes     = tabManager->sizes();
    int        current   = 0; // sum of current splitter sizes
    int        preferred = 0; // sum of preferred content size
    int        idx       = 0;
    foreach( TabWidget * widget, tabManager->getTabWidgets() )
    {
        current += sizes.at( idx );
        int preferredWidth = widget->sizeHint().width();
        sizes[ idx ] = preferredWidth;
        preferred   += preferredWidth;
        idx++;
    }

    int pad = this->width() - current; // get space without tab widget contents, e.g. splitter control, paddings
    pad += 10;
    resize( preferred + pad, height() );
    tabManager->setSizes( sizes );
}

// ------------ setting handler implementation ----------------------------------

/** saves settings that do not depend on an experiment */
void
MainWidget::saveGlobalSettings( QSettings& settings )
{
    // save position and size of the main window
    settings.setValue( "coords/size", size() );
    settings.setValue( "coords/pos", pos() );

    // save ValueView settings
    foreach( cube::DataType type, Globals::valueViews.keys() )
    {
        ValueView* current = Globals::valueViews.value( type );
        settings.setValue( "DataType" + QString::number( type ), current->getName() );
    }

    settings.setValue( "showSyncToolBar", syncAction->isChecked() );

    TreeConfig::getInstance()->saveGlobalSettings( settings );
    tabManager->saveGlobalSettings( settings );

    // save precision values
    PrecisionWidget* precisionWidget = Globals::getPrecisionWidget();
    settings.setValue( "precision/prec0", precisionWidget->getPrecision( FORMAT_TREES ) );
    settings.setValue( "precision/round0", precisionWidget->getRoundNr( FORMAT_TREES ) );
    settings.setValue( "precision/exp0", precisionWidget->getUpperExpNr( FORMAT_TREES ) );
    settings.setValue( "precision/prec1", precisionWidget->getPrecision( FORMAT_DEFAULT ) );
    settings.setValue( "precision/round1", precisionWidget->getRoundNr( FORMAT_DEFAULT ) );
    settings.setValue( "precision/exp1", precisionWidget->getUpperExpNr( FORMAT_DEFAULT ) );

    // save external file name
    settings.setValue( "lastExternalFileName", lastExternalFileName );
    // save file size threshold above which dynamic loading is enabled
    settings.setValue( "dynloadThreshold", dynloadThreshold );

    settings.setValue( "lastColorMapName", lastColorMapName );
}

void
MainWidget::loadGlobalStartupSettings( QSettings& settings )
{
    // restore names of recent files
    QStringList files = settings.value( "openedFiles" ).toStringList();
    for ( int i = files.size() - 1; i >= 0; --i )
    {
        if ( files[ i ].startsWith( "cube://" ) || QFile( files[ i ] ).exists() )
        {
            rememberFileName( files[ i ] );
        }
    }
    openedUrls = settings.value( "openedURLs" ).toStringList();

    StyleSheetEditor::getInstance()->loadGlobalStartupSettings( settings );
}

void
MainWidget::saveGlobalStartupSettings( QSettings& settings )
{
    settings.setValue( "openedFiles", openedFiles );
    settings.setValue( "openedURLs",  openedUrls );
    StyleSheetEditor::getInstance()->saveGlobalStartupSettings( settings );
}

void
MainWidget::loadGlobalSettings( QSettings& settings )
{
    // restore window geometry only on startup or if settings "restoreGeometry" is set
    if ( !_initGeometry )
    {
        _initGeometry = true;
        // restore position and size of the main window
        resize( settings.value( "coords/size", QSize( 600, 500 ) ).toSize() );
        move( settings.value( "coords/pos", QPoint( 200, 200 ) ).toPoint() );
    }

    if ( cube )
    {
        for ( int type = 1; type < 18; type++ ) // values for enum cube::DataType
        {
            QString name = settings.value( tr( "DataType" ) + QString::number( type ), "" ).toString();
            if ( !name.isEmpty() )
            {
                ValueView* view = PluginManager::getInstance()->getValueView( name );
                if ( view )
                {
                    Globals::valueViews.insert( ( cube::DataType )type, view );
                }
            }
        }
    }

    bool sync = settings.value( "showSyncToolBar", false ).toBool();
    syncAction->setChecked( sync );

    TreeConfig::getInstance()->loadGlobalSettings( settings );
    tabManager->loadGlobalSettings( settings );

    // restore precision values
    PrecisionWidget* precisionWidget = Globals::getPrecisionWidget();
    precisionWidget->setPrecision( settings.value( "precision/prec0", 2 ).toInt(), FORMAT_TREES );
    precisionWidget->setUpperExpNr( settings.value( "precision/exp0", 4 ).toInt(), FORMAT_TREES );
    precisionWidget->setRoundNr( settings.value( "precision/round0", 7 ).toInt(), FORMAT_TREES );
    precisionWidget->setPrecision( settings.value( "precision/prec1", 2 ).toInt(), FORMAT_DEFAULT );
    precisionWidget->setUpperExpNr( settings.value( "precision/exp1", 4 ).toInt(), FORMAT_DEFAULT );
    precisionWidget->setRoundNr( settings.value( "precision/round1", 7 ).toInt(), FORMAT_DEFAULT );

    // restore the threshold above which dynamic metric loading is enabled
    dynloadThreshold = settings.value( "dynloadThreshold", 1000000000 ).toInt();

    lastColorMapName = settings.value( "lastColorMapName", "" ).toString();
    // should be safe - the call to settings is AFTER initializing tabs
    loadColorMap();
}

void
MainWidget::loadStatus( QSettings& settings )
{
    loadGlobalSettings( settings );
}

void
MainWidget::saveStatus( QSettings& settings )
{
    saveGlobalSettings( settings );
}

QString
MainWidget::settingName()
{
    return "Settings";
}


void
MainWidget::saveExperimentSettings( QSettings& experimentSettings )
{
    tabManager->saveExperimentSettings( experimentSettings );
}

void
MainWidget::loadExperimentSettings( QSettings& experimentSettings )
{
    tabManager->loadExperimentSettings( experimentSettings );
}


void
MainWidget::loadColorMap()
{
    QList<ColorMap*> maps = PluginManager::getInstance()->getColorMaps();
    foreach( ColorMap * map, maps )
    {
        if ( lastColorMapName == map->getMapName() )
        {
            setColorMap( map );
            break;
        }
    }
}

void
MainWidget::dragEnterEvent( QDragEnterEvent* event )
{
    const QMimeData* mime = event->mimeData();
    if ( mime->hasUrls() &&  mime->text().startsWith( "file://" ) && ( mime->text().endsWith( ".cube" ) || mime->text().endsWith( ".cubex" ) || mime->text().endsWith( ".cube.gz" ) ) )
    {
        event->acceptProposedAction();
    }
}

void
MainWidget::dropEvent( QDropEvent* event )
{
    QString file = event->mimeData()->text();
    file.remove( "file://" );
    loadFile( file );
    event->acceptProposedAction();
}

//=======================================================================================
ReadCubeThread::ReadCubeThread( cube::CubeProxy* proxy )
{
    this->cube = proxy;
}

void
ReadCubeThread::run()
{
    QString errorMsg;

    if ( !cube )
    {
        return;
    }
    try
    {
        cube->openReport();
    }
    catch ( const std::exception& e  )
    {
        errorMsg = QString( tr( "Error. \n" ) ).append( QString( e.what() ) );
    }
    emit cubeLoaded( errorMsg );
}

// one needs these calls to mockup QThread
#ifdef CUBE_WEBASSEMBLY

void
ReadCubeThread::start()
{
    run();
}

#endif
