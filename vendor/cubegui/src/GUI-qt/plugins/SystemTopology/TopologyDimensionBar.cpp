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
#include <QBitmap>
#include <QIntValidator>
#include <QDebug>
#include "TopologyDimensionBar.h"

TopologyDimensionBar::TopologyDimensionBar( const std::vector<long>&        dims,
                                            const std::vector<std::string>& dimnames,
                                            QWidget*                        parent ) :  QWidget( parent ), dims_( dims )
{
    order            = 0;
    selection        = 0;
    modeSelection    = 0;
    foldBut          = 0;
    sliceBut         = 0;
    axisLabel        = 0;
    splitLengthInput = 0;
    axisToggled      = false;

    QHBoxLayout* mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins( 0, 0, 0, 0 );
    setLayout( mainLayout );
    mainLayout->addSpacing( 10 );

    int usedDimensions = 0;
    foreach( long dim, dims )
    {
        usedDimensions += ( dim > 1 );
    }
    if ( usedDimensions == 0 )
    {
        return; // only one element -> no splitting possible
    }
    bool isOneDimensional = ( usedDimensions == 1 );

    if ( isOneDimensional )    // panel for one-dimensional topologies
    {
        setWhatsThis( tr( "This widget controls how one-dimensional topologies are displayed. To have a better"
                          " overview of large one-dimensional topologies, the elements can be split into"
                          " chunks, which are displayed side by side." ) );

        QPixmap       pix( ":/images/split_small.png" );
        const QBitmap mask    = pix.createMaskFromColor( QColor( 255, 255, 255, 0 ).rgb() ); // transparent background for white
        QSize         pixSize = QSize( 48, 48 );
        pix.setMask( mask );
        pix              = pix.scaled( pixSize, Qt::KeepAspectRatio );
        toggleAxisButton = new QPushButton();
        toggleAxisButton->setIcon( QIcon( pix ) );
        toggleAxisButton->setIconSize( pixSize );
        mainLayout->addWidget( toggleAxisButton, 0, Qt::AlignHCenter  );
        connect( toggleAxisButton, SIGNAL( pressed() ), this, SLOT( onToggleAxis() ) );

        mainLayout->addWidget( new QLabel( tr( "split after n elements:" ) ), 0, Qt::AlignHCenter  );
        splitLengthInput = new QLineEdit();
        QFontMetrics metrics( splitLengthInput->font() );
        splitLengthInput->setFixedSize( metrics.boundingRect( "88888888" ).width(), splitLengthInput->sizeHint().height() );

        QIntValidator* validator = new QIntValidator( 0, 999999, this );
        splitLengthInput->setValidator( validator );
        mainLayout->addWidget( splitLengthInput, 0, Qt::AlignHCenter  );
        connect( splitLengthInput, SIGNAL( returnPressed() ), this, SLOT( handleSplitLengthChanged() ) );
    }
    else // panel for topologies with two and more dimensions
    {
        setWhatsThis( tr( "This widget controls how multi-dimensional topologies are mapped to an (x,y) or (x,y,z) presentation."
                          " It allows one either to fold multiple dimensions together on the same axis or"
                          " slice fixing coordinates of some dimensions."
                          " In fold mode, drag the dimension labels to the desired axis row in the table."
                          " In slice mode, two or three dimensions can have all of their coordinates shown"
                          " (on the axis whose label has been dragged beneath them) while a specific coordinate"
                          " can be chosen from the other dimensions using the menu above each dimension label."
                          " When the widget is wider than the panel, the contents can be scrolled horizontally"
                          " with the right and left arrow keys (after it has been selected)." ) );



        modeSelection = new QStackedLayout();

        // fill dnames with dimension names or enumerate them
        QStringList dnames;
        if ( dimnames.size() == dims.size() )
        {
            for ( uint i = 0; i < dimnames.size(); ++i )
            {
                dnames.append( QString( dimnames.at( i ).c_str() ) );
            }
        }
        else
        {
            for ( uint i = 0; i < dims.size(); ++i )
            {
                dnames.append( QString::number( i + 1 ) );
            }
        }


        QGroupBox* groupBox = new QGroupBox();
        groupBox->setStyleSheet( tr( "border:0;" ) );
        foldBut = new QRadioButton( "fold", this );
        foldBut->setChecked( true );
        sliceBut = new QRadioButton( tr( "slice" ), this );
        QVBoxLayout* vbox = new QVBoxLayout();
        vbox->setContentsMargins( 0, 0, 0, 0 );
        vbox->addWidget( foldBut );
        vbox->addWidget( sliceBut );
        groupBox->setLayout( vbox );
        groupBox->setFlat( true );
        connect( foldBut, SIGNAL( clicked() ), this, SLOT( foldingDimensionsChanged() ) );
        connect( sliceBut, SIGNAL( clicked() ), this, SLOT( selectedDimensionsChanged() ) );

        QWidget* stacked = new QWidget( this );
        stacked->setContentsMargins( 0, 0, 0, 0 );
        stacked->setLayout( modeSelection );

        order = new OrderWidget( dims, dnames );
        order->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
        if ( dims.size() == 2 )
        {
            sliceBut->setVisible( false ); // slice mode makes no sence for 2D Topologies
            foldBut->setVisible( false );
        }

        // axis label
        axisLabel = new QLabel();
        setAxisLabel( ( dims.size() == 2 ) ? ":/images/folding_xy_small.png" : ":/images/folding_xyz_small.png"  ); // default for folding mode

        // ------ dimension selection mode -------
        selection = new DimensionSelectionWidget( dims, dnames );
        connect( selection, SIGNAL( selectedDimensionsChanged() ), this, SLOT( selectedDimensionsChanged() ) );

        // minimize layout
        QWidget*     selectionWidget = new QWidget( this );
        QHBoxLayout* selL            = new QHBoxLayout();
        selL->setContentsMargins( 0, 0, 0, 0 );
        selL->addWidget( selection );
        selectionWidget->setLayout( selL );

        // ------ folding mode -------
        connect( order, SIGNAL( foldingDimensionsChanged() ), this, SLOT( foldingDimensionsChanged() ) );

        QWidget*     foldWidget = new QWidget( this );
        QHBoxLayout* fold       = new QHBoxLayout();
        fold->setContentsMargins( 0, 0, 0, 0 );
        fold->addWidget( order );
        foldWidget->setLayout( fold );

        // ------ -------
        modeSelection->addWidget( foldWidget );
        modeSelection->addWidget( selectionWidget );

        // ------ -------
        mainLayout->addWidget( axisLabel, 0, Qt::AlignHCenter  );
        mainLayout->addWidget( groupBox, 0, Qt::AlignHCenter  );
        mainLayout->addWidget( stacked, 0, Qt::AlignHCenter  );
        mainLayout->addStretch( 1 );
    }
    this->setMaximumHeight( minimumSizeHint().height() );
}

std::vector<std::vector<int> >
TopologyDimensionBar::getFoldingVector()
{
    if ( order )
    {
        return order->getFoldingVector();
    }
    else
    {
        return std::vector<std::vector<int> >();
    }
}

void
TopologyDimensionBar::setAxisLabel( QString icon )
{
    QPixmap pix( icon );
    axisLabel->setPixmap( pix.scaled( QSize( 60, 60 ), Qt::KeepAspectRatio ) );
}

/**
   emit dimensionsChanged, if the selected values are valid (3 full dimensions)
 */
void
TopologyDimensionBar::selectedDimensionsChanged()
{
    modeSelection->setCurrentIndex( foldBut->isChecked() ? 0 : 1 );
    std::vector<long> val = selection->getSelectionVector();
    emit              selectedDimensionsChanged( val );

    // count number of selected dimensions
    int dims = 0;
    for ( uint i = 0; i < val.size(); i++ )
    {
        if ( val[ i ] < 0 )
        {
            dims++;
        }
    }
    setAxisLabel( ( dims == 2 ) ? ":/images/projection_xy_small.png" : ":/images/projection_xyz_small.png" );
}
/**
   emit dimensionsChanged, if new combinations of dimensions are selected
 */
void
TopologyDimensionBar::foldingDimensionsChanged()
{
    modeSelection->setCurrentIndex( foldBut->isChecked() ? 0 : 1 );
    std::vector<std::vector<int> > val = order->getFoldingVector();
    emit                           foldingDimensionsChanged( val );
    int                            dims = 0;
    for ( uint i = 0; i < val.size(); i++ )
    {
        if ( val[ i ].size() != 0 )
        {
            dims++;
        }
    }
    setAxisLabel( ( dims == 2 ) ? ":/images/folding_xy_small.png" : ":/images/folding_xyz_small.png" );
}

void
TopologyDimensionBar::handleSplitLengthChanged()
{
    QString str = splitLengthInput->text();
    bool    ok;
    int     length = str.toInt( &ok );
    if ( ok )
    {
        emit splitLengthChanged( length );
    }
}


void
TopologyDimensionBar::onToggleAxis()
{
    emit toggleAxis();
    axisToggled = !axisToggled;

    QPixmap       pix( ":/images/split_small.png" );
    const QBitmap mask = pix.createMaskFromColor( QColor( 255, 255, 255, 0 ).rgb() ); // transparent background for white
    pix.setMask( mask );
    QSize pixSize = QSize( 48, 48 );
    pix = pix.scaled( pixSize, Qt::KeepAspectRatio );
    if ( axisToggled )
    {
        pix = pix.transformed( QTransform().rotate( 90 ) ).transformed( QTransform().scale( -1, 1 ) );
    }

    toggleAxisButton->setIcon( QIcon( pix ) );
}
/************************ settings ***************************/

//save topology settings
//
void
TopologyDimensionBar::saveSettings( QSettings& settings, int topologyId  )
{
    if ( ( selection == 0 ) || ( dims_.size() < 2 ) )
    {
        return;
    }

    QString groupName( "systemTopology" );
    groupName.append( QString::number( topologyId ) );
    settings.beginGroup( groupName );

    settings.setValue( "selectMode", sliceBut->isChecked() );

    if ( dims_.size() > 2 )
    {
        std::vector<long> selectedDimensions = selection->getSelectionVector();
        QList<QVariant>   save1;
        for ( uint i = 0; i < selectedDimensions.size(); ++i )
        {
            save1.append( ( int )selectedDimensions[ i ] );
        }
        settings.setValue( "selectedDimensions", QVariant( save1 ) );
    }

    QList<QVariant>                save2;
    std::vector<std::vector<int> > merge = order->getFoldingVector();
    for ( uint i = 0; i < merge.size(); ++i )
    {
        QList<QVariant> line;
        for ( uint j = 0; j < merge[ i ].size(); ++j )
        {
            line.append( merge[ i ][ j ] );
        }
        save2.append( QVariant( line ) );
    }
    settings.setValue( "mergedDimensions", QVariant( save2 ) );

    //settings.setValue( "userDefinedMinMaxValues", userDefinedMinMaxValues );
    //settings.setValue( "userMinValue", userMinValue );
    //settings.setValue( "userMaxValue", userMaxValue );

    settings.endGroup();
}

bool
TopologyDimensionBar::loadSettings( QSettings& settings, int topologyId )
{
    if ( ( selection == 0 ) || ( dims_.size() < 2 ) )
    {
        return false;
    }

    QString groupName = "systemTopology";
    groupName.append( QString::number( topologyId ) );
    settings.beginGroup( groupName );

    typedef enum { SELECT, FOLD } Mode;
    bool slice = settings.value( "selectMode" ).toBool();

    if ( dims_.size() > 2 )
    {
        QList<QVariant> list = settings.value( "selectedDimensions" ).toList();

        std::vector<long> selectedDimensions;
        for ( int i = 0; i < list.size(); i++ )
        {
            selectedDimensions.push_back( list[ i ].toInt() );
        }
        selection->setSelectionVector( selectedDimensions );
    }

    QList<QVariant>                list2 = settings.value( "mergedDimensions" ).toList();
    std::vector<std::vector<int> > foldingVec;
    for ( int i = 0; i < list2.size(); ++i )
    {
        QList<QVariant>  line = list2[ i ].toList();
        std::vector<int> foldLine;
        for ( int j = 0; j < line.size(); ++j )
        {
            foldLine.push_back( line[ j ].toInt() );
        }
        foldingVec.push_back( foldLine );
    }

    settings.endGroup();

    order->setFoldingVector( foldingVec );

    if ( slice )
    {
        sliceBut->setChecked( true );
        selectedDimensionsChanged();
    }
    else
    {
        foldingDimensionsChanged();
    }
    //userDefinedMinMaxValues = settings.value( "userDefinedMinMaxValues", false ).toBool();
    //userMinValue            = settings.value( "userMinValue", 0.0 ).toDouble();
    //userMaxValue            = settings.value( "userMaxValue", 0.0 ).toDouble();

    return true;
}
