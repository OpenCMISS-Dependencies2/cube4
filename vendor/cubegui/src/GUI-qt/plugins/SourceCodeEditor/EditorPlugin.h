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


#ifndef SourceCodeEditorPlugin_H
#define SourceCodeEditorPlugin_H
#include <QtGui>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QLineEdit>
#include <QStackedWidget>
#include <QScrollArea>
#include <QDialog>
#include <QLabel>
#include "TabInterface.h"
#include "CubePlugin.h"
#include "PluginServices.h"
#include "SourceCodeEditor.h"

namespace editor_plugin
{
class SourceInfo
{
public:
    cubepluginapi::TreeItem* item;
    QString                  originalLocation; // file name of the source code which corresponds to the selected region
    QString                  fileName;         // originalLocation with corrected path
    int                      startLine;        // begin of the region
    int                      endLine;          // end of the region
    bool
    isEmpty()
    {
        return fileName.isEmpty();
    }
};

class EditorPlugin : public QObject, cubepluginapi::TabInterface, cubepluginapi::CubePlugin, cubepluginapi::SettingsHandler
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "SourceCodeEditorPlugin" )
#endif


public:
    EditorPlugin()
    {
        deamonStarted = false;
    }

    // CubePlugin interface
    virtual QString
    name() const;

    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service );
    virtual void
    cubeClosed();

    void
    version( int& major,
             int& minor,
             int& bugfix ) const;

    virtual QString
    getHelpText() const;

    virtual QString
    getHelpURL() const
    {
        return "SourceCodeViewerPlugin.html";
    }

    // SettingsHandler
    virtual void
    loadExperimentSettings( QSettings& );

    virtual void
    saveExperimentSettings( QSettings& );

    virtual void
    saveGlobalSettings( QSettings& settings );
    virtual void
    loadGlobalSettings( QSettings& settings );

    virtual QString
    settingName()
    {
        return "SourceCodeViewer";
    }

    // TabInterface implementation
    virtual QWidget*
    widget();
    virtual QString
    label() const;
    virtual void
    setActive( bool active );
    virtual void
    detachEvent( QMainWindow*,
                 bool isDetached );

    QIcon
    icon() const;

    // end TabInterface implementation

private slots:
    void
    onChangeFont();
    void
    onSaveFile();
    void
    onSaveFileAs();
    void
    onToggleReadOnly( bool checked );
    void
    showSourceCode();
    void
    search( const QString& );
    void
    searchForward();
    void
    searchBackward();
    void
    closeSourceView();
    void
    treeItemSelected( cubepluginapi::TreeItem* );
    void
    openExternalEditor();
    void
    openDefinedExternalEditor();
    void
    openFileDialog();
    void
    deleteProcess();
    void
    onChooseEditor();
    void
    startSearch();
    void
    contextMenuIsShown( cubepluginapi::DisplayType type,
                        cubepluginapi::TreeItem*   item );
    void
    toFront();

private:
    QStackedWidget* mainWidget;
    QWidget*        editorWidget;   // widget to display source
    QScrollArea*    openWidget;     // shown if source cannot be loaded
    // this is the editor to display and edit source code files
    SourceCodeEditor* textEdit;
    QWidget*          searchWidget; // widget below textEdit that is shown if search context menu item is selected
    // Selected Font
    QFont    fontSourceCode;
    QAction* font;
    // these are two buttons that get enabled and disabled when toggling the readOnly status
    QAction* textEditSaveAction, * textEditSaveAsAction;
    QAction* readOnlyAction;
    QAction* findAction;
    QAction* externalAction;     // open in external editor
    QAction* externalUserAction; // open in user specific external editor
    // "Find" - input field
    QLineEdit* findEdit;
    // these are two buttons for search forwards or backwards
    QPushButton* findNextButton, * findPrevButton;
    QPushButton* openFile;
    QLabel*      openFileLabel;

    cubepluginapi::PluginServices* service;
    cubepluginapi::TreeItem*       selectedItem; // currently selected item

    SourceInfo source;                           // source information about the currently selected item
    QString    origPath;                         // original location of the sources
    QString    userPath;                         // the new location of the sources
    QString    searchText;                       // holds "search" text

    QList<QStringList> sourcePathes;             // default pathes to search for sources

    QHash<QString, QStringList> externalEditors;
    QString                     externalEditor;
    bool                        deamonStarted;  // status of process of external editor

    //-------------------------------------------------------------------
    void
    showSourceCode( const QString& sourceFile,
                    int            startLine,
                    int            endLine );
    QString
    getSourceFile( const QString& sourceFile );
    void
    updateActions();
    void
    createWidgets();
    void
    setSourceInfo();
    void
    addPathReplacement( const QString& orig,
                        const QString& user );
};
} // namespace
#endif // SourceCodeEditorPlugin_H
