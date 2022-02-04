/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2021                                                     **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include "CubeRegion.h"
#include "CubeVertex.h"
#include "CubeCnode.h"

#include "CallTreeLabelDialog.h"
#include "StyleSheetEditor.h"
#include "TabManager.h"
#include "TreeConfig.h"
#include "Globals.h"
#include "Tree.h"
#include "DefaultCallTree.h"
#include "TreeItem.h"

#ifdef CXXABI_H_AVAILABLE
#include <cxxabi.h>
#else
#ifdef DEMANGLE_H_AVAILABLE
#define HAVE_DECL_BASENAME 1
#include <demangle.h>
#endif
#endif

#include "Compatibility.h"
#ifdef HAS_QREGULAR_EXPRESSION
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

using namespace cubegui;

TreeConfig* TreeConfig::single = 0;

TreeConfig::TreeConfig()
{
}


TreeConfig*
TreeConfig::getInstance()
{
    return single ? single : single = new TreeConfig();
}

QMenu*
TreeConfig::getMenu()
{
    QMenu* treeMenu = new QMenu( tr( "Trees" ) );
    treeMenu->setStatusTip( tr( "Ready" ) );
    treeMenu->setEnabled( false );

    QAction* fontAct = new QAction( tr( "Fo&nt and colors..." ), this );
    connect( fontAct, SIGNAL( triggered() ),  StyleSheetEditor::getInstance(), SLOT( configureStyleSheet() ) );
    fontAct->setWhatsThis( tr( "Opens a dialog to set fonts, colors and other settings with Qt Stylesheets." ) );
    treeMenu->addAction( fontAct );

    QAction* markerAction = PluginManager::getInstance()->getMarkerConfigAction();
    treeMenu->addAction( markerAction );

    treeMenu->addSeparator(); // -- call tree or call flat specific
    demangleAction = new QAction( tr( "Demangle Function Names" ), this );
    demangleAction->setCheckable( true );
    treeMenu->addAction( demangleAction );
    connect( demangleAction, SIGNAL( toggled( bool ) ), this, SLOT( enableDemangling( bool ) ) );
#if defined( CXXABI_H_AVAILABLE ) || defined( DEMANGLE_H_AVAILABLE )
    demangleAction->setEnabled( true );
#endif

    QAction* labelAction = new QAction( tr( "Shorten Function Names..." ), this );
    treeMenu->addAction( labelAction );
    connect( labelAction, SIGNAL( triggered() ), this, SLOT( configureCallTreeLabel() ) );

    treeMenu->addSeparator(); // system tree specific
    systemLabelAction = new QAction( tr( "Append rank to system tree items" ), this );
    systemLabelAction->setCheckable( true );
    treeMenu->addAction( systemLabelAction );
    connect( systemLabelAction, SIGNAL( toggled( bool ) ), this, SLOT( enableSystemLabelRank( bool ) ) );

    return treeMenu;
}

void
TreeConfig::enableDemangling( bool enable )
{
    callConfig.demangleFunctions = enable;
    updateCallTreeLabel();
}

void
TreeConfig::enableSystemLabelRank( bool enable )
{
    appendSystemRank = enable;
    Tree* tree = Globals::getTabManager()->getActiveTree( SYSTEM );
    if ( !tree )
    {
        return;
    }

    tree->invalidateItemDisplayNames();
    tree = Globals::getTabManager()->getActiveTree( SYSTEM );
    tree->invalidateItemDisplayNames();
    Globals::getTabManager()->updateTreeItemProperties();
}

void
TreeConfig::updateCallDisplayConfig( const CallDisplayConfig& newConfig )
{
    callConfig = newConfig;
    updateCallTreeLabel();
}

void
TreeConfig::configureCallTreeLabel()
{
    CallTreeLabelDialog* d = new CallTreeLabelDialog( callConfig, Globals::getMainWindow() );
    connect( d, SIGNAL( configChanged( const CallDisplayConfig & ) ), this, SLOT( updateCallDisplayConfig( const CallDisplayConfig & ) ) );
    d->setVisible( true );
}

// invalidates the displayName of all call tree and call flat tree items
void
TreeConfig::updateCallTreeLabel()
{
    for ( Tree* tree : Globals::getTabManager()->getTrees() )
    {
        tree->invalidateItemDisplayNames();
    }
    Globals::getTabManager()->updateTreeItemProperties();
}

void
TreeConfig::saveGlobalSettings( QSettings& settings )
{
    settings.setValue( "calltree/hideArguments", callConfig.hideArguments );
    settings.setValue( "calltree/hideReturnValue", callConfig.hideReturnValue );
    settings.setValue( "calltree/hideClassHierarchy", callConfig.hideClassHierarchy );
    settings.setValue( "calltree/hideModules", callConfig.hideModules );
    settings.setValue( "calltree/enableDemangling", callConfig.demangleFunctions );
    settings.setValue( "systemtree/appendRank", appendSystemRank );
}

void
TreeConfig::loadGlobalSettings( QSettings& settings )
{
    callConfig.hideArguments      = settings.value( "calltree/hideArguments", false ).toBool();
    callConfig.hideReturnValue    = settings.value( "calltree/hideReturnValue", false ).toBool();
    callConfig.hideClassHierarchy = settings.value( "calltree/hideClassHierarchy", 0 ).toInt();
    callConfig.hideModules        = settings.value( "calltree/hideModules", false ).toBool();
    callConfig.demangleFunctions  = settings.value( "calltree/enableDemangling", true ).toBool();
    appendSystemRank              = settings.value( "systemtree/appendRank", false ).toBool();

    demangleAction->setChecked( callConfig.demangleFunctions );
    systemLabelAction->setChecked( appendSystemRank );
}

/**
 * @brief TreeItem::createDisplayName
 * @param any tree item
 * @return the name of the tree item adapted to the user settings, e.g. demangling, ...
 */
QString
TreeConfig::createDisplayName( TreeItem* item )
{
    if ( item->getDisplayType() == CALL )
    {
        return createCallDisplayName( item );
    }
    else if ( item->getDisplayType() == SYSTEM )
    {
        return createSystemDisplayName( item );
    }
    else
    {
        return item->getOriginalName();
    }
}

QString
TreeConfig::createSystemDisplayName( TreeItem* item )
{
    if ( !appendSystemRank || item->getType() != LOCATIONITEM )
    {
        return item->getOriginalName();
    }
    TreeItem* parent = item->getParent();
    if ( parent && parent->getType() == LOCATIONGROUPITEM )
    {
        const QString  mpi  = "MPI Rank ";
        const QString& name = parent->getOriginalName();
        if ( name.startsWith( mpi ) )
        {
            QString number = name.right( name.size() - mpi.size() );
            return QString().append( item->getOriginalName() ).append( ":" ).append( number );
        }
    }
    else
    {
        return ""; // tree is not yet initialised
    }
    return item->getOriginalName();
}

/*
 * remove the namespace/class part of the name
 * removes all "::name" from orig but keeps the last keepLevel
 */
static QString
removeNamespace( const QString& orig, int keepLevel )
{
    QString shortName = orig;

    int lastColon = shortName.lastIndexOf( "::", -1 );
    while ( keepLevel >= 0 && lastColon > 0 )
    {
        int index      = shortName.length();
        int colon      = lastColon;
        int colonCount = 0;
        while ( colon > 0 )
        {
            QChar c;
            index = colon - 1;
            do // search left for next char that isn't part of an identifier
            {
                c = shortName.at( index );
                index--;
            }
            while ( index > 0 && ( c.isLetterOrNumber() || c == '_' ) );

            if ( index == 0 )
            {
                colon = -1;
            }
            else if ( c == ':' && shortName.at( index ) == ':' ) // found next :: on the left
            {
                colon = index;
            }
            else
            {
                colon  = -1;
                index += 2;
            }
            if ( ++colonCount <= keepLevel ) // show the first level classes/namespaces
            {
                lastColon = colon;
            }
        }
        if ( lastColon > 0 )
        {
            shortName.remove( index, lastColon - index + 2 );
            lastColon = shortName.lastIndexOf( "::", index - shortName.length() );
        }
    }
    return shortName;
}

/** Searches for a blank left of startIdx. Characters between pairs of <> are ignored
 * @returns position of the blank */
static int
indexOfLeftBlank( const QString& str, int startIdx )
{
    int bracket = 0;
    for ( int i = startIdx; i > 0; i-- )
    {
        if ( str.at( i ) == '>' )
        {
            bracket++;
        }
        else if ( str.at( i ) == '<' )
        {
            bracket--;
        }
        else if ( bracket == 0 &&  str.at( i ) == ' ' )
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief TreeItem::createCallDisplayName
 * changes the original call tree/flat tree label acoording to the settings in CallDisplayConfig
 * - demangled c++ function names
 * - C++ function names without argument list, return value, templates
 * - C++ function names with limited class/namespace hierarchy
 * - fortran subroutines without module names
 * @returns the name that will be displayed in the tree view
 */
QString
TreeConfig::createCallDisplayName( TreeItem* item )
{
    QString displayName = item->getOriginalName();

    CallDisplayConfig& config     = callConfig;
    cube::Vertex*      cubeObject = item->getCubeObject();
    if ( config.demangleFunctions &&  ( static_cast<cube::Cnode* > ( cubeObject ) ) != NULL && ( static_cast<cube::Cnode* > ( cubeObject ) )->get_callee()  != NULL )
    {
        std::string dName, mName;
        if ( item->getType() == CALLITEM )
        {
            cube::Cnode* cnode = static_cast<cube::Cnode* > ( cubeObject );
            dName = cnode->get_callee()->get_name();
            mName = cnode->get_callee()->get_mangled_name();
        }
        else if ( item->getType() == REGIONITEM )
        {
            cube::Region* region = static_cast<cube::Region* > ( cubeObject );
            dName = region->get_name();
            mName = region->get_mangled_name();
        }
        bool isMangled = ( dName.rfind( "_Z", 0 ) == 0 ) || ( dName.rfind( "__Z", 0 ) );
        if ( isMangled && dName == mName )
        {
#ifdef CXXABI_H_AVAILABLE
            int   status;
            char* demangled = abi::__cxa_demangle( dName.c_str(), 0, 0, &status );
            if ( status == 0 )
            {
                displayName = demangled;
                free( demangled );
            }
#else
#ifdef DEMANGLE_H_AVAILABLE
            char* demangled = cplus_demangle( dName.c_str(), DMGL_PARAMS );
            if ( demangled )
            {
                displayName = demangled;
                free( demangled );
            }
#endif
#endif
        }
    }

    bool shorten = true;

    if ( config.hideClassHierarchy == -1 && !config.hideArguments && !config.hideReturnValue && !config.hideModules )
    {
        shorten = false;
    }

    if ( cubeObject ) // cubeObject is null for iterations
    {                 // check, if region should be ignored because it isn't user code
        cube::Cnode*  cnode  = dynamic_cast<cube::Cnode*>( cubeObject );
        cube::Region* region = 0;
        if ( cnode )
        {
            region = dynamic_cast<cube::Region*>( cnode->get_callee() );
        }
        if ( region && region->get_paradigm() == "unknown" ) // old format, paradigm isn't set
        {
            if ( displayName.contains( "!$omp" ) )
            {
                shorten = false;
            }
        }
        else if ( region && region->get_paradigm() != "compiler" )
        {
            shorten = false; // only shorten regions with compiler tag, ignore e.g. !$omp
        }
    }

    if ( shorten )
    {
        QString shortName = displayName;
#ifdef HAS_QREGULAR_EXPRESSION
        shortName.remove( QRegularExpression( "\\[with(.*)\\]" ) ); // e.g. "FT getValue(const DictionaryDatum&, Name) [with FT = lockPTRDatum...]"
#else
        shortName.remove( QRegExp( "\\[with(.*)\\]" ) );            // e.g. "FT getValue(const DictionaryDatum&, Name) [with FT = lockPTRDatum...]"
#endif

        //========= for C/C++ subroutines =========
        int argIdx = -1; // starting position of the argument list, -1 => not found

        QString op    = "operator(";
        int     start = displayName.indexOf( op );
        start = ( start > 0 ) ? start + op.length() : 0; // "operator(" is not start of a function -> search next '('

        int idx = shortName.indexOf( '(', start );
        if ( ( idx > 0 ) && ( shortName.indexOf( '(', idx + 1 ) == -1 ) )
        {
            // simplified: if only one parenthesis exist -> start of argument list found
            argIdx = idx;
        }
        if ( argIdx != -1 )             // argument list found
        {
            if ( config.hideArguments ) // don't show argument list, ignore everything right of the '('
            {
                shortName = displayName.left( argIdx );
            }

            if ( config.hideReturnValue )
            {
                // first blank left of the function name which doesn't belong to a template is the
                int startFunctionIdx = indexOfLeftBlank( displayName, argIdx ) + 1;
                shortName.remove( 0, startFunctionIdx );
            }
        }

        if ( config.hideClassHierarchy > -1 )
        {
            // remove text between <> (templates)
            int startIdx = shortName.indexOf( "<" );
            int count    = -2;
            while ( startIdx >= 0 )
            {
                int endIdx = startIdx;
                count = 1;
                while ( count > 0 && ++endIdx < shortName.length() )
                {
                    QChar c = shortName.at( endIdx );
                    if ( c == '<' )
                    {
                        count++;
                    }
                    else if ( c == '>' )
                    {
                        count--;
                    }
                }
                if ( count == 0 )
                {
                    shortName.remove( startIdx, endIdx - startIdx + 1 );
                    startIdx = shortName.indexOf( "<" );
                }
                else // brackets are not matching
                {
                    startIdx = -1;
                }
            }
            shortName = removeNamespace( shortName, config.hideClassHierarchy );
        }

        //========= for Fortran subroutines =========
        if ( config.hideModules )
        {
            idx = displayName.indexOf( '@' ); // e.g. !$omp parallel @jacobi.F90:58
            if ( idx < 0 )
            {
                idx = displayName.indexOf( '.' );
                if ( idx > 0 )
                {
                    // Intel ifort: "<modulename>." prefix
                    shortName.remove( 0, idx + 1 );
                }
                else
                {
                    // IBM XLF: "__<modulename>_NMOD_" prefix
                    idx = displayName.indexOf( "_NMOD_" );
                    if ( idx > 0 )
                    {
                        shortName.remove( 0, idx + 6 );
                    }
                }
            }
        }
        displayName = shortName;
    } // shorten

    if ( item->getDisplayType() == CALL )
    {
        DefaultCallTree* defaultCallTree = dynamic_cast<DefaultCallTree*> ( item->getTree() );
        if ( defaultCallTree && item == defaultCallTree->getLoopRootItem() ) // non-aggreated loop root
        {
            displayName.append( tr( " [loop]" ) );
        }
    }

    return displayName;
}
