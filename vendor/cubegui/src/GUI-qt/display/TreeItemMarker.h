/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef TREEITEMMARKER_H
#define TREEITEMMARKER_H

#include <QColor>
#include <QPixmap>

namespace cubepluginapi
{
class PluginServices;
}

namespace cubegui
{
class MarkerLabel;
class TreeItem;
class MarkerAttributes
{
public:
    MarkerAttributes()
    {
    };
    MarkerAttributes( const QColor& c );

    const QColor&
    getColor() const;

    /** reset to original color scheme */
    static void
    resetColors();

    friend class TreeItemMarkerDialog;
    friend class TreeItemMarker;

private:
    void
    setColor( const QColor& color );
    void
    release();
    static MarkerAttributes
    getMarkerAttributes();

    static QList<MarkerAttributes > definedMarker;
    QColor                          color;
};

/**
 * TreeItemMarker is used to visually mark tree items in the view.
 */
class TreeItemMarker
{
    friend class cubepluginapi::PluginServices;
    friend class TreeItemMarkerDialog;
public:
    ~TreeItemMarker();
    const MarkerAttributes&
    getAttributes() const;
    QString
    getLabel( const TreeItem* item = 0 ) const;
    const QPixmap
    getIcon() const;
    void
    setSize( int iconHeight );

    bool
    isColorVisible() const
    {
        return showColor;
    }
    bool
    isIconVisible() const
    {
        return showIcon;
    }
    static bool
    isGrayedOut()
    {
        return grayItems;
    }
    bool
    isInsignificant() const
    {
        return insignificant;
    }

private:
    // only allow PluginServices to create markers
    TreeItemMarker( const QString&        label,
                    const QList<QPixmap>& icons = QList<QPixmap>( ),
                    bool                  insignificant = false,
                    MarkerLabel*          markerLabel = 0 );
    void
    setColorVisible( bool visible );
    void
    setIconVisible( bool visible );

    QString          label;
    MarkerAttributes attribute;
    MarkerLabel*     markerLabel;
    QList<QPixmap>   icons; // different sizes of the displayed icon
    QPixmap          currentIcon;
    bool             showColor;
    bool             showIcon;
    bool             insignificant;
    static bool      grayItems; // if true, gray out all items without markers
};
}
#endif // TREEITEMMARKER_H
