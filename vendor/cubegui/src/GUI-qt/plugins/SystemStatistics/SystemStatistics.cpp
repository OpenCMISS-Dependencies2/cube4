/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include <QAbstractButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QSlider>
#include <QRadioButton>
#include <iostream>

#include "SystemStatistics.h"
#include "PluginManager.h"
#include "StatisticalInformation.h"

using namespace std;
using namespace system_statistics;
using namespace cubepluginapi;
using cubegui::StatisticalInformation;
using cubegui::BoxPlot;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( SystemStatisticsPlugin, SystemStatistics );
#endif

const QString SystemStatistics::VIOLIN_HELP =
    tr( " Violin Plot is a method of plotting numeric data. It is a box plot with a rotated kernel density plot on each side. "
        " Typically violin plots will include a marker for the median of the data and a box indicating the interquartile range. "
        " Overlaid on this box plot is a kernel density estimation." );

const QString SystemStatistics::BOX_HELP =
    tr( " The Boxplot shows the distribution of values in the system tree."
        " It starts with the smallest value (minimum) and ends with the largest value (maximum)."
        " The bottom and top of the box represent the lower quartile (Q1) and the upper quartile (Q3)."
        " The bold line represents the median (Q2) and the dashed line represents the mean value."
        " To zoom into the boxplot, select an area with the mouse."
        " To zoom out, click with the middle mouse button in the drawing." );

/** set a version number, the plugin with the highest version number will be loaded */
void
SystemStatistics::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
SystemStatistics::getHelpText() const
{
    return tr( "This plugins displays statistics as box or violin plot.\n\n" ) + BOX_HELP + "\n\n" + VIOLIN_HELP;
}

// plugin name
QString
SystemStatistics::name() const
{
    return "System Statistics";
}

/**
   creates a bot plot which displays the values calculated by SystemTreeWidget
 */

SystemStatistics::SystemStatistics()
{
    boxplot    = 0;
    violinplot = 0;
}

bool
SystemStatistics::cubeOpened( PluginServices* service )
{
    this->service = service;

    boxplot = new BoxPlot( 0 );
    boxplot->setWhatsThis( BOX_HELP );

    violinplot = new ViolinPlot( 0 );
    violinplot->setWhatsThis( VIOLIN_HELP );

    QWidget* buttons = new QWidget;   //parent widget for the buttons
    // graph_sel = new QWidget;          //widget for boxplot plus the kernel selector;
    QRadioButton* vp       = new QRadioButton( tr( "Violin Plot" ) );
    QRadioButton* bp       = new QRadioButton( tr( "Box Plot" ) );
    QHBoxLayout*  h_layout = new QHBoxLayout;
    bp->setChecked( true );

    h_layout->addWidget( bp );
    h_layout->addWidget( vp );
    buttons->setLayout( h_layout );
    buttons->setMaximumHeight( buttons->sizeHint().height() );

    //creating the stacked plots
    stacker = new QStackedWidget;
    stacker->addWidget( boxplot );
    stacker->addWidget( violinplot );

    container   = new QSplitter( Qt::Vertical );
    subsetCombo = new QComboBox();
    subsetCombo->setMaximumHeight( subsetCombo->sizeHint().height() );

    container->addWidget( stacker );
    container->addWidget( buttons );
    container->addWidget( subsetCombo );

    // set lower splitter element to minimum size (1 pixel => replaced by minimumSize())
    QList<int> sizeList;
    sizeList << stacker->size().height() << 1 << 1;
    container->setSizes( sizeList );

    connect( service, SIGNAL( orderHasChanged( const QList<cubepluginapi::DisplayType> & ) ),
             this, SLOT( orderHasChanged( const QList<cubepluginapi::DisplayType> & ) ) );

    connect( bp, SIGNAL( toggled( bool ) ), this,  SLOT( showBoxPlot( bool ) ) );
    connect( vp, SIGNAL( toggled( bool ) ), this,  SLOT( showViolinPlot( bool ) ) );

    service->addTab( SYSTEM, this );

    return true;
}

void
SystemStatistics::cubeClosed()
{
    delete container;
}

QWidget*
SystemStatistics::widget()
{
    return container;
}

QString
SystemStatistics::label() const
{
    return "Statistics";
}

void
SystemStatistics::valuesChanged()
{
    vector<double> values = getValues( false );

    // fill box plot with calculated data
    if ( boxplot->isVisible() )
    {
        StatisticalInformation item( values );
        StatisticalInformation abs;

        double start;
        if ( service->getValueModus() == ABSOLUTE_VALUES )
        {
            abs   = item;
            start = item.getMinimum();
        }
        else
        {
            vector<double> absoluteValues = getValues( true );
            abs   = StatisticalInformation( absoluteValues );
            start = 0.0;
        }

        boxplot->Set( item, abs, start );
    }
    else if ( violinplot->isVisible() )
    {
        ViolinStatistics       vitem( values );
        StatisticalInformation abs;

        double start;
        if ( service->getValueModus() == ABSOLUTE_VALUES )
        {
            abs   = vitem.getStatistics();
            start = abs.getMinimum();
        }
        else
        {
            vector<double> absoluteValues = getValues( true );
            abs   = StatisticalInformation( absoluteValues );
            start = 0.0;
        }
        violinplot->set( vitem, abs, start );
    }
    if ( boxplot->isVisible() || violinplot->isVisible() )
    {
        updateValueWidget();
        updateSubsetCombo(); // visited count may have changed
    }
}

QIcon
SystemStatistics::icon() const
{
    return QIcon( ":images/boxplot-icon.png" );
}

vector<double>
SystemStatistics::getValues( bool absolute )
{
    vector<double> values;
    double         value;
    ValueModus     modus = service->getValueModus();

    // calculate the sum of all inclusive values of the selected subset
    foreach( TreeItem * item, service->getActiveSubset() )
    {
        if ( absolute )
        {
            value = item->getTotalValue();
        }
        else
        {
            if ( item->isExpanded() )
            {
                bool ok;
                value = item->calculateValue( modus, ok, false ); // calculate inclusive value
            }
            else
            {
                value = item->getValue();
            }
        }
        values.push_back( value );
    }

    return values;
}

/**
   updates the value widget below the tab widget
 */
void
SystemStatistics::updateValueWidget()
{
    vector<double>         values = getValues( service->getValueModus() == ABSOLUTE_VALUES );
    StatisticalInformation item( values );

    if ( item.getCount() == 0 )
    {
        service->clearValueWidget();
    }
    else
    {
        if ( service->getValueModus() == ABSOLUTE_VALUES )
        {
            double displayedValue = item.getMean();
            // in the absolute value mode display the mean value (with the min/max values)
            service->updateValueWidget( this, item.getMinimum(), item.getMaximum(), displayedValue );
        }
        else
        {
            vector<double>         absValues = getValues( true );
            StatisticalInformation abs( absValues );

            double displayedValue = std::nan( "" );    // dont't show first row
            double sigma          = sqrt( abs.getVariance() );

            // in value modes other than the absolute value mode display the
            // current minvalue/value/maxvalue plus absolute
            // minvalue/value/maxvalue
            service->updateValueWidget( this, 0.0, 100.0, displayedValue,
                                        abs.getMinimum(), abs.getMaximum(), displayedValue,
                                        abs.getMean(), sigma );
        }
    }
}

void
SystemStatistics::orderHasChanged( const QList<cubepluginapi::DisplayType>& order )
{
    bool enabled = order.at( 0 ) != SYSTEM; // disable statistics, if system widget is first
    service->enableTab( this, enabled );
}

void
SystemStatistics::setActive( bool active )
{
    if ( active )
    {
        updateSubsetCombo();
        valuesChanged();
    }
}

//slot for box plot
void
SystemStatistics::showBoxPlot( bool enabled )
{
    if ( enabled )
    {
        stacker->setCurrentIndex( 0 );
        valuesChanged();
    }
}

//slot fot violin plot
void
SystemStatistics::showViolinPlot( bool enabled )
{
    if ( enabled )
    {
        stacker->setCurrentIndex( 1 );
        valuesChanged();
    }
}

/** Updates the labels of the subset combo and select the active subset. No signals are emited.
 */
void
SystemStatistics::updateSubsetCombo()
{
    subsetCombo->disconnect();
    subsetModel.setStringList( service->getSubsetLabelList() );
    subsetCombo->setModel( &subsetModel );
    subsetCombo->setCurrentIndex( service->getActiveSubsetIndex() );
    connect( subsetCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( subsetChanged( int ) ) );
}

void
SystemStatistics::subsetChanged( int idx )
{
    service->setActiveSubset( idx );
    valuesChanged();
    updateSubsetCombo();                // update number of elements for "Visited"
}
