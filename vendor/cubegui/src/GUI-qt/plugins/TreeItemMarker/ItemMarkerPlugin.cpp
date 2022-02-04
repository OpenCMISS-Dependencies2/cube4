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
#include <QtPlugin>
#include <QInputDialog>
#include "ItemMarkerPlugin.h"
#include "PluginServices.h"
#include "TreeItemMarker.h"
#include "DefaultMarkerLabel.h"

/**
 * If the plugin doesn't load, start cube with -verbose to get detailed information.
 * If the error message "Plugin verification data mismatch" is printed, check if the
 * same compiler and Qt version have been used.
 */

using namespace cubepluginapi;
using namespace itemmarkerplugin;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( TreeItemMarkerPlugin, ItemMarkerPlugin ); // ( PluginName, ClassName )
#endif

bool
ItemMarkerPlugin::cubeOpened( PluginServices* service )
{
    this->service = service;
    connect( service, SIGNAL( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ),
             this, SLOT( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ) );

    markerLabel = new DefaultMarkerLabel( tr( "Tree item marker" ) );
    marker      = service->getTreeItemMarker( tr( "Tree item marker" ), QList<QPixmap>(), false, markerLabel );

    service->addSettingsHandler( this );

    return true; // initialisation is ok => plugin should be shown
}

void
ItemMarkerPlugin::cubeClosed()
{
    markedItems.clear();
    delete markerLabel;
}


/**
 * @brief DemoPlugin::contextMenuIsShown is called, if the user clicks with right mouse button on a tree
 * item.
 */
void
ItemMarkerPlugin::contextMenuIsShown( DisplayType type, TreeItem* item )
{
    contextItem = item;

    if ( item )
    {
        if ( !markedItems.contains( item ) )
        {
            QAction* contextAction = service->addContextMenuItem( type, tr( "Mark this item" ) );
            connect( contextAction, SIGNAL( triggered() ), this, SLOT( setMarker() ) );
        }
        else
        {
            QAction* contextAction = service->addContextMenuItem( type, tr( "Remove marker" ) );
            connect( contextAction, SIGNAL( triggered() ), this, SLOT( removeMarker() ) );
            contextAction = service->addContextMenuItem( type, tr( "Set marker label" ) );
            connect( contextAction, SIGNAL( triggered() ), this, SLOT( editMarker() ) );
        }
    }
}

void
ItemMarkerPlugin::removeMarker()
{
    service->removeMarker( contextItem, marker );
    service->updateTreeView( contextItem->getDisplayType() );
    markedItems.removeAll( contextItem );
}

void
ItemMarkerPlugin::setMarker()
{
    service->addMarker( contextItem, marker );
    service->updateTreeView( contextItem->getDisplayType() );
    markedItems.append( contextItem );
}

void
ItemMarkerPlugin::editMarker()
{
    bool    ok;
    QString text = QInputDialog::getText( service->getParentWidget(), tr( "Set marker label" ),
                                          tr( "Insert label:" ), QLineEdit::Normal, markerLabel->getLabel( contextItem ), &ok );
    if ( ok && !text.isEmpty() )
    {
        markerLabel->setLabel( contextItem, text );
    }
}

/** set a version number, the plugin with the highest version number will be loaded */
void
ItemMarkerPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

/** unique plugin name */
QString
ItemMarkerPlugin::name() const
{
    return "TreeItem Marker";
}

QString
ItemMarkerPlugin::getHelpText() const
{
    return tr( "This Plugin allows the user to mark tree items." );
}

void
ItemMarkerPlugin::loadExperimentSettings( QSettings& settings )
{
    QList<QVariant> list      = settings.value( "marked" ).toList();
    QList<QVariant> labelList = settings.value( "markedLabel" ).toList();

    for ( int i = 0; i < list.size(); i++ )
    {
        TreeItem* item  = TreeItem::convertQVariantToTreeItem( list.at( i ) );
        QString   label = labelList.at( i ).toString();
        if ( item )
        {
            contextItem = item;
            setMarker();
            markerLabel->setLabel( contextItem, label );
        }
    }
}

void
ItemMarkerPlugin::saveExperimentSettings( QSettings& settings )
{
    QList<QVariant> list;
    QStringList     labelList;
    foreach( TreeItem * item, markedItems )
    {
        QVariant data = item->convertToQVariant();
        list.append( data );
        labelList.append( markerLabel->getLabel( item ) );
    }
    settings.setValue( "marked", QVariant( list ) );
    settings.setValue( "markedLabel", QVariant( labelList ) );
}
