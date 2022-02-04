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


#include "config.h"
#include <cassert>
#include <QMessageBox>
#include <QFileDialog>
#include <QDialog>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QApplication>
#include <QFontDialog>
#include <QMenuBar>
#include <QStyle>
#include <QLabel>
#include <QScrollBar>
#include <QScrollArea>
#include <QStringList>
#include "Compatibility.h"
#ifdef HAS_QREGULAR_EXPRESSION
#include <QRegularExpression>
#define REGULAR_EXPRESSION QRegularExpression
#else
#include <QRegExp>
#define REGULAR_EXPRESSION QRegExp
#endif

#include "TreeItem.h"
#include "EditorPlugin.h"
#include "CPPSyntaxHighlighter.h"
#include "FortranSyntaxHighlighter.h"
#include "PythonSyntaxHighlighter.h"
#include "EditorConfig.h"

using namespace cubepluginapi;
using namespace editor_plugin;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( SourceCodeEditorPlugin, EditorPlugin ); // ( PluginName, ClassName )
#endif

#define ADD_LINES 20

// OPARI_FNAME: workaround for gfortran bug, see #805
#define OPARI_FNAME ".input.prep.opari"

void
EditorPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
EditorPlugin::name() const
{
    return "Source Code Viewer";
}

QString
EditorPlugin::getHelpText() const
{
    return tr( "Source code viewer with syntax highlighting and basic editing functionality" );
}

bool
EditorPlugin::cubeOpened( PluginServices* service )
{
    this->service = service;
    service->addSettingsHandler( this );

    createWidgets();

    // --- create actions -----------------------------------------------------------------------
    font = new QAction( tr( "Set Font..." ), editorWidget ); // select code editor font
    connect( font, SIGNAL( triggered() ), this, SLOT( onChangeFont() ) );

    textEditSaveAction = new QAction( tr( "Save" ), editorWidget );
    connect( textEditSaveAction, SIGNAL( triggered() ), this, SLOT( onSaveFile() ) );
    textEditSaveAsAction = new QAction( tr( "Save as" ), editorWidget );
    connect( textEditSaveAsAction, SIGNAL( triggered() ), this, SLOT( onSaveFileAs() ) );

    readOnlyAction = new QAction( tr( "Read only" ), editorWidget );
    readOnlyAction->setCheckable( true );
    readOnlyAction->setChecked( true );
    connect( readOnlyAction, SIGNAL( toggled( bool ) ), this, SLOT( onToggleReadOnly( bool ) ) );

    QAction* chooseEditor = new QAction( tr( "Set external editor" ), editorWidget );
    connect( chooseEditor, SIGNAL( triggered() ), this, SLOT( onChooseEditor() ) );

    findAction = new QAction( tr( "&Find" ), editorWidget );
    findAction->setShortcutContext( Qt::WidgetWithChildrenShortcut );
    findAction->setShortcut( QKeySequence( Qt::CTRL | Qt::Key_F ) );
    editorWidget->addAction( findAction );
    findAction->setToolTip( tr( "Find string" ) );
    connect( findAction, SIGNAL( triggered( bool ) ), this, SLOT( startSearch() ) );

    externalAction = new QAction( tr( "Open in &external editor" ), this );
    connect( externalAction, SIGNAL( triggered( bool ) ), this, SLOT( openExternalEditor() ) );
    externalUserAction = new QAction( tr( "" ), this );
    connect( externalUserAction, SIGNAL( triggered( bool ) ), this, SLOT( openDefinedExternalEditor() ) );
    externalUserAction->setVisible( false );

    // -------------------------------------------------------------------------------------
    connect( service, SIGNAL( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ),
             this, SLOT( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ) );

    QMenu* menu = service->enablePluginMenu();
    menu->addAction( font );
    menu->addAction( textEditSaveAction );
    menu->addAction( textEditSaveAsAction );
    menu->addAction( readOnlyAction );
    menu->addAction( chooseEditor );

    textEdit->addToContextMenu( findAction );
    textEdit->addToContextMenu( externalAction );
    textEdit->addToContextMenu( externalUserAction );

    service->addTab( SYSTEM, this, OTHER_PLUGIN_TAB );

    onToggleReadOnly( readOnlyAction->isChecked() );

    return true;
}


void
EditorPlugin::contextMenuIsShown( cubepluginapi::DisplayType type, cubegui::TreeItem* )
{
    if ( type == CALL )
    {
        QAction* action = service->addContextMenuItem( CALL, tr( "Show source code" ) );
        connect( action, SIGNAL( triggered( bool ) ), this, SLOT( toFront() ) );
    }
}

void
EditorPlugin::toFront()
{
    service->toFront( this );
}

void
EditorPlugin::createWidgets()
{
    mainWidget   = new QStackedWidget();
    editorWidget = new QWidget();
    textEdit     = new SourceCodeEditor();

    // -------------------------------------------------------------------------------------
    searchWidget = new QWidget();
    searchWidget->setVisible( false );
    findPrevButton = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_ArrowLeft ), "" );
    findPrevButton->setToolTip( tr( "Find previous" ) );
    connect( findPrevButton, SIGNAL( clicked( bool ) ), this, SLOT( searchBackward() ) );
    findNextButton = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_ArrowRight ), "" );
    findNextButton->setToolTip( tr( "Find next" ) );
    connect( findNextButton, SIGNAL( clicked( bool ) ), this, SLOT( searchForward() ) );
    findEdit = new QLineEdit();
    connect( findEdit, SIGNAL( textChanged( QString ) ), this, SLOT( search( const QString & ) ) );

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* hideButton   = new QPushButton(  QApplication::style()->standardIcon( QStyle::SP_DockWidgetCloseButton ), "" );
    hideButton->setToolTip( tr( "Close search widget" ) );
    connect( hideButton, SIGNAL( clicked( bool ) ), searchWidget, SLOT( hide() ) );

    buttonLayout->setContentsMargins( 0, 0, 0, 0 );
    buttonLayout->addWidget( new QLabel( tr( "Find" ) ) );
    buttonLayout->addWidget( findEdit );
    buttonLayout->addWidget( findPrevButton );
    buttonLayout->addWidget( findNextButton );
    buttonLayout->addWidget( hideButton );
    searchWidget->setLayout( buttonLayout );
    // -------------------------------------------------------------------------------------

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( textEdit );
    layout->addWidget( searchWidget );

    editorWidget->setLayout( layout );
    editorWidget->setMinimumSize( 50, 50 );
    textEdit->setMinimumSize( 50, 50 );

    // -------------------------------------------------------------------------------------
    openFile = new QPushButton( tr( "Open Source File" ) ); // to be used in getSourceFile
    connect( openFile, SIGNAL( pressed() ), this, SLOT( openFileDialog() ) );
    openFileLabel = new QLabel( tr( "file" ) );

    QWidget*     buttonWidget = new QWidget();
    QHBoxLayout* l            = new QHBoxLayout();
    buttonWidget->setLayout( l );
    openFile->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    l->addWidget( openFile );
    l->addStretch();
    QWidget* child = new QWidget();
    layout = new QVBoxLayout();
    child->setLayout( layout );
    layout->addSpacerItem( new QSpacerItem( 0, 200, QSizePolicy::Minimum, QSizePolicy::Preferred ) );
    layout->addWidget( openFileLabel );
    layout->addWidget( buttonWidget );
    layout->setSizeConstraint( QLayout::SetMinimumSize ); // required for resizing if label content changes

    openWidget = new QScrollArea();
    openWidget->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    openWidget->setWidget( child );

    mainWidget->addWidget( editorWidget );
    mainWidget->addWidget( openWidget );
}

void
EditorPlugin::cubeClosed()
{
}

void
EditorPlugin::onChooseEditor()
{
    new EditorConfig( editorWidget, externalEditors, externalEditor );
    updateActions();
}

void
EditorPlugin::updateActions()
{
    bool enabled = !readOnlyAction->isChecked() && !source.isEmpty();
    textEditSaveAction->setEnabled( enabled );
    textEditSaveAsAction->setEnabled( enabled );
    if ( !externalEditor.isEmpty() && externalEditors.contains( externalEditor ) )
    {
        externalUserAction->setText( tr( "Open in " ) + externalEditor );
        externalUserAction->setVisible( true );
    }
    else
    {
        externalUserAction->setVisible( false );
    }
}

void
EditorPlugin::closeSourceView()
{
    /*
       service->removeTab( this );
       delete mainWidget;
       mainWidget = 0;
     */
}

QStringList
splitCommand( QString str )
{
    return str.split( " " );
}

// open dialog to choose an external editor and open the file on success
void
EditorPlugin::openExternalEditor()
{
    setSourceInfo();
    if ( source.isEmpty() )
    {
        return;
    }
    onChooseEditor();
    openDefinedExternalEditor();
}

// open file in previously defined editor
void
EditorPlugin::openDefinedExternalEditor()
{
    setSourceInfo();
    if ( source.isEmpty() )
    {
        return;
    }

    QStringList commands = externalEditors.value( externalEditor );
    if ( commands.isEmpty() )
    {
        return;
    }

    commands = commands.replaceInStrings( "%LINE%", QString::number( source.startLine ) );
    commands = commands.replaceInStrings( "%SOURCE%", source.fileName );

    if ( !deamonStarted )
    {
        QStringList startDeamon = commands.at( 0 ).split( " " );
        if ( !startDeamon.isEmpty() )
        {
            QProcess* process    = new QProcess();
            QString   executable = startDeamon.takeFirst();
            process->start( executable, startDeamon );
            process->waitForFinished();
            if ( process->exitCode() == 0 )
            {
                deamonStarted = true;
            }
            delete process;
        }
    }
    QStringList open = commands.at( 1 ).split( " " );
    if ( !open.isEmpty() )
    {
        QProcess* process    = new QProcess();
        QString   executable = open.takeFirst();
        process->start( executable, open );
        connect( process, SIGNAL( finished( int ) ), this, SLOT( deleteProcess() ) );
    }
}

void
EditorPlugin::deleteProcess()
{
    QProcess* process = ( QProcess* )this->sender();
    process->deleteLater();
}

static QString
replacePath( const QString& path, const QString& origPath, const QString& userPath )
{
    QString replaced = path;
    if ( origPath.length() > 0 )
    {
        replaced.replace( origPath, userPath );
    }
    else
    {
        replaced = userPath + path;
    }
    return replaced;
}

void
EditorPlugin::openFileDialog()
{
    QString fileName      = source.originalLocation.trimmed();
    QString fileNameLower = fileName;
    fileNameLower.replace( QString( OPARI_FNAME ) + ".F", ".f" );
    fileName.replace( OPARI_FNAME, "" );

    if ( !userPath.isEmpty() )                       // user has already inserted a new source directory
    {
        fileName = replacePath( fileName, origPath, userPath );
    }
    else // take the path of the cube file as default directory
    {
        QString dir  = QFileInfo( service->getCubeFileName() ).absoluteDir().absolutePath();
        QString file = QFileInfo( fileName ).fileName();
        fileName = dir.append( QDir::separator() ).append( file );
    }

    // show a dialog to enter a filename
    QString caption = "";
    QString filter  = QFileInfo( fileName ).fileName() + ";;" + QFileInfo( fileNameLower ).fileName() + ";;*";
    fileName = QFileDialog::getOpenFileName( service->getParentWidget(), caption, fileName, filter );
    if ( !fileName.isEmpty() )
    {
        if ( source.originalLocation.contains( OPARI_FNAME ) )
        {
            QFileInfo origF( source.originalLocation );
            QFileInfo userF( fileName );
            source.originalLocation = origF.absolutePath() + QDir::separator() + userF.fileName();
        }
        // the path of the source files has changed -> remember old and new path
        QStringList originalL = source.originalLocation.trimmed().split( '/' );
        QStringList userPathL = fileName.split( '/' );
        QStringList inCommon;
        while ( originalL.length() > 0 && userPathL.length() > 0 )
        {
            QString last1 = originalL.takeLast();
            QString last2 = userPathL.takeLast();
            if ( last1 != last2 )
            {
                break;
            }
            inCommon.prepend( last1 );
        }
        if ( inCommon.size() > 0 )
        {
            QString common = inCommon.join( "/" );
            origPath = source.originalLocation.trimmed();
            origPath.remove( common );
            userPath = fileName;
            userPath.remove( common );
            addPathReplacement( origPath, userPath );
        }
        else
        {
            origPath = "";
            userPath = "";
        }
        showSourceCode();
    }
}

void
EditorPlugin::addPathReplacement( const QString& orig, const QString& user )
{
    QStringList newPath;
    newPath << orig << user;
    foreach( const QStringList &path, sourcePathes )
    {
        QString origPath = path.first();
        if ( orig == origPath )
        {
            sourcePathes.removeOne( path );
            break;
        }
    }
    sourcePathes.push_front( newPath );
}

/**
   Checks if the given (original) source file exists. If it doesn't exit and the user has already inserted another location, then
   the filename is adapted to the new path.
 */
QString
EditorPlugin::getSourceFile( const QString& sourceFile )
{
    QString fileName = sourceFile.trimmed();

    if ( !userPath.isEmpty() ) // path replacement for this experiment exists
    {
        fileName = replacePath( fileName, origPath, userPath );
    }
    else // try path replacements from global settings
    {
        QFile file, fileLower;
        foreach( QStringList path, sourcePathes )
        {
            QString origPath = path.takeFirst();
            QString userPath = path.takeFirst();
            fileName = replacePath( sourceFile.trimmed(), origPath, userPath );

            QString fileNameLower = fileName;
            fileNameLower.replace( QString( OPARI_FNAME ) + ".F", ".f" );
            fileName.replace( OPARI_FNAME, "" );

            file.setFileName( fileName );
            fileLower.setFileName( fileNameLower );
            if ( file.exists() || fileLower.exists() )
            {
                this->origPath = origPath;
                this->userPath = userPath;
                fileName       = file.exists() ? file.fileName() : fileLower.fileName();
                break;
            }
        }
    }

    if ( source.originalLocation.contains( OPARI_FNAME ) )
    {
        QString fileNameLower = fileName;
        fileNameLower.replace( QString( OPARI_FNAME ) + ".F", ".f" );
        QFile file( fileNameLower );
        if ( file.exists() )
        {
            fileName = fileNameLower;
        }
        else
        {
            fileName.replace( OPARI_FNAME, "" );
        }
    }

    QFile file( fileName );
    if ( !file.exists() || !file.open( QFile::ReadOnly | QFile::Text ) )
    {
        // file doesn't exist -> show message in main widget and add button to call a file dialog
        openFile->setVisible( true );
        QString message;
        if ( fileName.isEmpty() )
        {
            message = tr( "No source information available" );
            openFile->setVisible( false );
        }
        else if ( !file.exists() )
        {
            message = tr( "File \"%1\" does not exist." ).arg( fileName );
            message.append( tr( "\n\nDo you want to open another file?" ) );
        }
        else
        {
            message = tr( "Cannot read file %1:\n%2." ).arg( fileName ).arg( file.errorString() );
            message.append( tr( "\n\nDo you want to open another file?" ) );
        }
        openFileLabel->setText( message );
        mainWidget->setCurrentIndex( 1 );
        mainWidget->setToolTip( "" );
        return "";
    }
    else   // file exists
    {
        mainWidget->setCurrentIndex( 0 );
        mainWidget->setToolTip( fileName );
        return fileName;
    }
}

void
EditorPlugin::setSourceInfo()
{
    source.item = selectedItem;

    cubepluginapi::TreeItem* infoItem = source.item;
    // get source info from current item
    infoItem->getSourceInfo( source.originalLocation, source.startLine, source.endLine );
    // if info is not available, search for source info in all parent items
    while ( source.originalLocation.isEmpty() && infoItem->getParent() )
    {
        infoItem = infoItem->getParent();
        infoItem->getSourceInfo( source.originalLocation, source.startLine, source.endLine );
    }

    source.fileName = getSourceFile( source.originalLocation );

    // only source of parent region found => search for current item in parent region
    if ( !source.fileName.isEmpty() && source.startLine >= 0 && infoItem != source.item )
    {
        QFile file( source.fileName );
        bool  found  = false;
        int   lineNr = 0;
        if ( file.open( QIODevice::ReadOnly ) )
        {
            QTextStream in( &file );
            while ( !in.atEnd() && ( ++lineNr < source.startLine ) )
            {
                in.readLine();
            }
            while ( !in.atEnd() && ( ++lineNr < source.endLine ) )
            {
                QString line = in.readLine();
                if ( line.contains( source.item->getName() ) )
                {
                    found = true;
                    lineNr--;
                    break;
                }
            }
            file.close();
        }
        if ( found )
        {
            source.startLine = lineNr;
            source.endLine   = lineNr;
        }
    }
}

static bool
regExpMatches( REGULAR_EXPRESSION regExp, const QString& source )
{
#ifdef HAS_QREGULAR_EXPRESSION
    return regExp.match( source ).hasMatch();
#else
    return regExp.indexIn( source ) != -1;
#endif
}

void
EditorPlugin::showSourceCode()
{
    setSourceInfo();
    if ( source.isEmpty() )
    {
        return;
    }

    QFile file( source.fileName );
    file.open( QIODevice::ReadOnly );
    QTextStream in( &file );
    textEdit->setText( in.readAll() );

    textEdit->setFont( fontSourceCode );
    textEdit->markRegion( source.startLine, source.endLine );

    // todo: line numbers

    REGULAR_EXPRESSION fortranSourceCode( "\\.[fF][:digit:]{0,2}$" );
    REGULAR_EXPRESSION pythonSourceCode( "\\.py$" );
    // REGULAR_EXPRESSION cSourceCode( "\\.c|h$" ); c/c++ is default
    // REGULAR_EXPRESSION cppSourceCode( "\\.cpp|hpp$" );

    enum { Cpp, Fortran, Python } language = Cpp;

    if ( regExpMatches( fortranSourceCode, source.fileName ) )
    {
        language = Fortran;
    }
    else if ( regExpMatches( pythonSourceCode, source.fileName ) )
    {
        language = Python;
    }

    if ( language == Cpp )
    {
        new CPPSyntaxHighlighter( textEdit->document() );
    }
    else if ( language == Fortran )
    {
        new FortranSyntaxHighlighter( textEdit->document() );
    }
    else if ( language == Python )
    {
        new PythonSyntaxHighlighter( textEdit->document() );
    }
}
// end of sourceCode()

// Selection of another font in source code dialog.
void
EditorPlugin::onChangeFont()
{
    QFont defaultFont = editorWidget ? textEdit->font() : QTextEdit().font();
    fontSourceCode = QFontDialog::getFont( 0, defaultFont );
    if ( editorWidget )
    {
        textEdit->setFont( fontSourceCode );
    }
}

// this slot is called by the editor for source code;
// it stores the eventually modified source code
//
void
EditorPlugin::onSaveFile()
{
    // "fileName" stores the name for the last source code opening/storing
    QFile file( source.fileName );

    // if the file cannot be opened for writing
    // display a warning message and return without storing
    if ( !file.open( QFile::WriteOnly | QFile::Text ) )
    {
        QString message = tr( "Cannot write file %1:\n%2." ).arg( source.fileName ).arg( file.errorString() );
        service->setMessage( message, Error );
        return;
    }

    {
        // write source code into the opened file
        QTextStream out( &file );
        QApplication::setOverrideCursor( Qt::WaitCursor );
        out << textEdit->toPlainText();
        QApplication::restoreOverrideCursor();
        out.flush();
    }
}


// this slot is called by the editor opened for source code;
// it stores the eventually modified source code under a new name
//
void
EditorPlugin::onSaveFileAs()
{
    // get the new file name by user input
    QString tmpName = QFileDialog::getSaveFileName( service->getParentWidget() );
    // tmpName can be empty, if the user canceled the input;
    // in this case just return without storing
    if ( tmpName.isEmpty() )
    {
        return;
    }
    // and call the slot for storing source code
    onSaveFile();
    // update the window title of the dialog widget containing the text editor
    editorWidget->setWindowTitle( tmpName ); // todo set window title if editor widget is detached
}

void
EditorPlugin::onToggleReadOnly( bool checked )
{
    if ( editorWidget )
    {
        textEdit->setReadOnly( checked );
    }
    updateActions();
}

void
EditorPlugin::search( const QString& text )
{
    searchText = text;
    searchBackward();
    searchForward();
}

void
EditorPlugin::searchForward()
{
    textEdit->find( searchText );
}

void
EditorPlugin::searchBackward()
{
    textEdit->find( searchText, QTextDocument::FindBackward );
}

// -------------------------------------------------
// implementation of settings handler interface
// -------------------------------------------------
void
EditorPlugin::loadExperimentSettings( QSettings& settings )
{
    origPath = settings.value( "origPath", "" ).toString();
    userPath = settings.value( "userPath", "" ).toString();
}

void
EditorPlugin::saveExperimentSettings( QSettings& settings )
{
    settings.setValue( "origPath", origPath );
    settings.setValue( "userPath", userPath );
}

void
EditorPlugin::loadGlobalSettings( QSettings& settings )
{
    // --- define default values for external editors
    externalEditors.clear();
    QStringList kate;
    kate << "" << "kate --line %LINE% %SOURCE%";
    externalEditors.insert( "kate", kate );
    QStringList emacs;
    emacs << "emacsclient -c -n --alternate-editor=" << "emacsclient -n +%LINE% %SOURCE%";
    externalEditors.insert( "emacs", emacs );
    QStringList gedit;
    gedit << "" << "gedit +%LINE% %SOURCE%";
    externalEditors.insert( "gedit", gedit );
    // todo emacs does not open new window, once it is closed, tries to open in terminal
    // --- end default values for external editors

    int size = settings.beginReadArray( "ExternalEditors" );
    for ( int i = 0; i < size; ++i )
    {
        settings.setArrayIndex( i );
        QStringList list   = settings.value( "editor" ).toString().split( "," );
        QString     editor = list.takeFirst();
        externalEditors.insert( editor, list );
    }
    settings.endArray();
    externalEditor = settings.value( "DefaultEditor", "" ).toString();

    sourcePathes.clear();
    size = settings.beginReadArray( "PathReplacement" );
    for ( int i = 0; i < size; ++i )
    {
        settings.setArrayIndex( i );
        QStringList path;
        path << settings.value( "origPath" ).toString() << settings.value( "userPath" ).toString();
        sourcePathes.append( path );
    }
    settings.endArray();
}

void
EditorPlugin::saveGlobalSettings( QSettings& settings )
{
    settings.beginWriteArray( "ExternalEditors" );
    int idx = 0;
    foreach( QString editor, externalEditors.keys() )
    {
        QStringList list = externalEditors.value( editor );
        list.prepend( editor );
        QString value = list.join( "," );
        settings.setArrayIndex( idx++ );
        settings.setValue( "editor", value );
    }
    settings.endArray();
    settings.setValue( "DefaultEditor", externalEditor );

    settings.beginWriteArray( "PathReplacement" );
    idx = 0;
    foreach( QStringList path, sourcePathes )
    {
        settings.setArrayIndex( idx++ );
        settings.setValue( "origPath", path.takeFirst() );
        settings.setValue( "userPath", path.takeFirst() );
        if ( idx > 10 )
        {
            break;             // only save latest 10 entries
        }
    }
    settings.endArray();
}

// -------------------------------------------------
// implementation of tab interface
// -------------------------------------------------

void
EditorPlugin::detachEvent( QMainWindow* window, bool isDetached )
{
    if ( isDetached )
    {
        QMenuBar* bar     = new QMenuBar( editorWidget );
        QMenu*    file    = bar->addMenu( tr( "&File" ) );
        QMenu*    display = bar->addMenu( tr( "&Display" ) );
        display->addAction( font );
        file->addAction( textEditSaveAction );
        file->addAction( textEditSaveAsAction );
        file->addAction( findAction );
        file->addAction( externalAction );
        file->addAction( readOnlyAction );

        window->layout()->setMenuBar( bar );
    }
}

QWidget*
EditorPlugin::widget()
{
    return mainWidget;
}

QString
EditorPlugin::label() const
{
    return tr( "Source" );
}

QIcon
EditorPlugin::icon() const
{
    return QIcon( ":/images/source.png" );
}

void
EditorPlugin::setActive( bool active )
{
    if ( active )
    {
        service->connect( service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ),
                          this, SLOT( treeItemSelected( cubepluginapi::TreeItem* ) ) );
        TreeItem* item = service->getSelection( CALL );
        treeItemSelected( item );
    }
    else
    {
        service->disconnect( service, SIGNAL( treeItemIsSelected( cubepluginapi::TreeItem* ) ),
                             this, SLOT( treeItemSelected( cubepluginapi::TreeItem* ) ) );
    }
}

void
EditorPlugin::treeItemSelected( cubepluginapi::TreeItem* item )
{
    if ( item->getDisplayType() != CALL )
    {
        return;
    }

    selectedItem = item;

    showSourceCode();
    updateActions();
}

void
EditorPlugin::startSearch()
{
    QTextCursor   cursor( textEdit->textCursor() );
    const QString selected = cursor.selectedText();
    if ( selected.size() > 0 )
    {
        findEdit->setText( selected );
    }
    searchWidget->setVisible( true );
}
