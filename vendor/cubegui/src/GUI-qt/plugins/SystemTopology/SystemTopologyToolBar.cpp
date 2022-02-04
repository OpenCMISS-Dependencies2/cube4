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


#include "SystemTopologyToolBar.h"
#include "PluginServices.h"
#include <QToolButton>
#include <QMessageBox>
#include <QInputDialog>
#include <assert.h>

using namespace cubepluginapi;

SystemTopologyToolBar::SystemTopologyToolBar( PluginServices* service ) : QToolBar( tr( "SystemTopology" ) )
{
    parent          = 0;
    recentSelection = 0;
    this->service   = service;

    leftAct  = this->addAction( QIcon( ":/images/left_small.png" ), QString( tr( "left" ) ) );
    rightAct = this->addAction( QIcon( ":/images/right_small.png" ), QString( tr( "right" ) ) );
    upAct    = this->addAction( QIcon( ":/images/up_small.png" ), QString( tr( "up" ) ) );
    downAct  = this->addAction( QIcon( ":/images/down_small.png" ), QString( tr( "down" ) ) );
    dist1Act = this->addAction( QIcon( ":/images/distance1_small.png" ), QString( tr( "dist+" ) ) );
    dist2Act = this->addAction( QIcon( ":/images/distance2_small.png" ), QString( tr( "dist-" ) ) );
    zoom1Act = this->addAction( QIcon( ":/images/zoomin_small.png" ), QString( tr( "zoom+" ) ) );
    zoom2Act = this->addAction( QIcon( ":/images/zoomout_small.png" ), QString( tr( "zoom-" ) ) );
    resetAct = this->addAction( QIcon( ":/images/reset_small.png" ), QString( tr( "reset" ) ) );
    scaleAct = this->addAction( QIcon( ":/images/scale_small.png" ), QString( tr( "scale" ) ) );
    userAct  = this->addAction( QIcon( ":/images/user_small.png" ), QString( tr( "min/max" ) ) );

    leftAct->setWhatsThis( tr( "Moves the whole topology to the left." ) );
    rightAct->setWhatsThis( tr( "Moves the whole topology to the right." ) );
    upAct->setWhatsThis( tr( "Moves the whole topology upwards." ) );
    downAct->setWhatsThis( tr( "Moves the whole topology downwards." ) );
    dist1Act->setWhatsThis( tr( "Increase the distance between the planes of the topology." ) );
    dist2Act->setWhatsThis( tr( "Decrease the distance between the planes of the topology." ) );
    zoom1Act->setWhatsThis( tr( "Enlarge the topology." ) );
    zoom2Act->setWhatsThis( tr( "Scale down the topology." ) );
    resetAct->setWhatsThis( tr( "Reset the display. It scales the topology such that it fits into the visible rectangle, and transforms it into a default position." ) );
    scaleAct->setWhatsThis( tr( "It scales the topology such that it fits into the visible rectangle, without transformations." ) );
    userAct->setWhatsThis( tr( " Similarly to the functions offered in the context menu of trees, you can activate and deactivate the application of user-defined minimal and maximal values for the color extremes, i.e., the values corresponding to the left and right end of the color legend. If the you activate user-defined values for the color extremes, you are asked to define two values that should correspond to the minimal and to the maximal colors. All values outside of this interval will get the color gray. Note that canceling any of the input windows causes no changes in the coloring method.  If user-defined min/max values are activated, the selected value information widget displays a \"(u)\" for \"user-defined\" behind the minimal and maximal color values." ) );

    xLabel = new QLabel( tr( "x-rot:" ) );
    this->addWidget( xLabel );

    xSpin = new QSpinBox();
    xSpin->setMinimum( 0 );
    xSpin->setMaximum( 359 );
    xSpin->setSingleStep( 1 );
    xSpin->setValue( 300 );
    this->addWidget( xSpin );

    yLabel = new QLabel( tr( "y-rot:" ) );
    this->addWidget( yLabel );

    ySpin = new QSpinBox();
    ySpin->setMinimum( 0 );
    ySpin->setMaximum( 359 );
    ySpin->setSingleStep( 1 );
    ySpin->setValue( 30 );
    this->addWidget( ySpin );

    xLabel->setWhatsThis( tr( "Rotate the topology cube about the x-axis with the defined angle." ) );
    xSpin->setWhatsThis( tr( "Rotate the topology cube about the x-axis with the defined angle." ) );
    yLabel->setWhatsThis( tr( "Rotate the topology cube about the y-axis with the defined angle." ) );
    ySpin->setWhatsThis( tr( "Rotate the topology cube about the y-axis with the defined angle." ) );

    QToolButton* leftButton  = ( QToolButton* )( this->widgetForAction( leftAct ) );
    QToolButton* rightButton = ( QToolButton* )( this->widgetForAction( rightAct ) );
    QToolButton* upButton    = ( QToolButton* )( this->widgetForAction( upAct ) );
    QToolButton* downButton  = ( QToolButton* )( this->widgetForAction( downAct ) );
    QToolButton* dist1Button = ( QToolButton* )( this->widgetForAction( dist1Act ) );
    QToolButton* dist2Button = ( QToolButton* )( this->widgetForAction( dist2Act ) );
    QToolButton* zoom1Button = ( QToolButton* )( this->widgetForAction( zoom1Act ) );
    QToolButton* zoom2Button = ( QToolButton* )( this->widgetForAction( zoom2Act ) );
    QToolButton* resetButton = ( QToolButton* )( this->widgetForAction( resetAct ) );
    QToolButton* scaleButton = ( QToolButton* )( this->widgetForAction( scaleAct ) );
    QToolButton* userButton  = ( QToolButton* )( this->widgetForAction( userAct ) );
    leftButton->setAutoRaise( false );
    rightButton->setAutoRaise( false );
    upButton->setAutoRaise( false );
    downButton->setAutoRaise( false );
    dist1Button->setAutoRaise( false );
    dist2Button->setAutoRaise( false );
    zoom1Button->setAutoRaise( false );
    zoom2Button->setAutoRaise( false );
    resetButton->setAutoRaise( false );
    scaleButton->setAutoRaise( false );
    userButton->setAutoRaise( false );

    leftAct->setToolTip( tr( "Move left" ) );
    rightAct->setToolTip( tr( "Move right" ) );
    upAct->setToolTip( tr( "Move up" ) );
    downAct->setToolTip( tr( "Move down" ) );
    dist1Act->setToolTip( tr( "Increase plane distance" ) );
    dist2Act->setToolTip( tr( "Decrease plane distance" ) );
    zoom1Act->setToolTip( tr( "Zoom in" ) );
    zoom2Act->setToolTip( tr( "Zoom out" ) );
    resetAct->setToolTip( tr( "Reset" ) );
    scaleAct->setToolTip( tr( "Scale into window" ) );
    userAct->setToolTip( tr( "Set minimum/maximum values for coloring" ) );

    leftButton->setAutoRepeat( true );
    rightButton->setAutoRepeat( true );
    upButton->setAutoRepeat( true );
    downButton->setAutoRepeat( true );
    dist1Button->setAutoRepeat( true );
    dist2Button->setAutoRepeat( true );
    zoom1Button->setAutoRepeat( true );
    zoom2Button->setAutoRepeat( true );

    connect( leftButton, SIGNAL( pressed() ), this, SLOT( moveLeft() ) );
    connect( rightButton, SIGNAL( pressed() ), this, SLOT( moveRight() ) );
    connect( upButton,   SIGNAL( pressed() ), this, SLOT( moveUp() ) );
    connect( downButton, SIGNAL( pressed() ), this, SLOT( moveDown() ) );
    connect( dist1Button, SIGNAL( pressed() ), this, SLOT( increasePlaneDistance() ) );
    connect( dist2Button, SIGNAL( pressed() ), this, SLOT( decreasePlaneDistance() ) );
    connect( zoom1Button, SIGNAL( pressed() ), this, SLOT( zoomIn() ) );
    connect( zoom2Button, SIGNAL( pressed() ), this, SLOT( zoomOut() ) );
    connect( resetButton, SIGNAL( pressed() ), this, SLOT( reset() ) );
    connect( scaleButton, SIGNAL( pressed() ), this, SLOT( scale() ) );
    connect( userButton, SIGNAL( pressed() ), this, SLOT( coloring() ) );

    connect( xSpin, SIGNAL( valueChanged( int ) ), this, SLOT( setXAngle( int ) ) );
    connect( ySpin, SIGNAL( valueChanged( int ) ), this, SLOT( setYAngle( int ) ) );

    disableTopologyButtons();
}

void
SystemTopologyToolBar::reinit()
{
    parent          = 0;
    recentSelection = 0;
}

void
SystemTopologyToolBar::setTopologyWidget( SystemTopologyWidget* newparent )
{
    disconnect( xSpin, SLOT( setValue( int ) ) );
    disconnect( ySpin, SLOT( setValue( int ) ) );

    parent = newparent;
    if ( parent == NULL )
    {
        disableTopologyButtons();
    }
    else
    {
        xSpin->setValue( parent->getTransform()->getXAngle() );
        ySpin->setValue( parent->getTransform()->getYAngle() );
        connect( parent->getTransform(), SIGNAL( xAngleChanged( int ) ), xSpin, SLOT( setValue( int ) ) );
        connect( parent->getTransform(), SIGNAL( yAngleChanged( int ) ), ySpin, SLOT( setValue( int ) ) );
        enableTopologyButtons();
    }
}

// set the button style of the topology tool bar;
// this slot is connected to actions in the menu Display/Toolbar

// slots connected to the actions in the topology tool bar

void
SystemTopologyToolBar::moveDown()
{
    if ( parent != NULL )
    {
        parent->getTransform()->moveDown();
    }
}
void
SystemTopologyToolBar::moveUp()
{
    if ( parent != NULL )
    {
        parent->getTransform()->moveUp();
    }
}
void
SystemTopologyToolBar::moveRight()
{
    if ( parent != NULL )
    {
        parent->getTransform()->moveRight();
    }
}
void
SystemTopologyToolBar::moveLeft()
{
    if ( parent != NULL )
    {
        parent->getTransform()->moveLeft();
    }
}

void
SystemTopologyToolBar::setXAngle( int angle )
{
    if ( parent != NULL )
    {
        parent->getTransform()->setXAngle( angle );
    }
}

void
SystemTopologyToolBar::setYAngle( int angle )
{
    if ( parent != NULL )
    {
        parent->getTransform()->setYAngle( angle );
    }
}

void
SystemTopologyToolBar::increasePlaneDistance()
{
    if ( parent != NULL )
    {
        parent->getTransform()->increasePlaneDistance();
    }
}
void
SystemTopologyToolBar::decreasePlaneDistance()
{
    if ( parent != NULL )
    {
        parent->getTransform()->decreasePlaneDistance();
    }
}
void
SystemTopologyToolBar::zoomIn()
{
    if ( parent != NULL )
    {
        parent->getTransform()->zoomIn();
    }
}
void
SystemTopologyToolBar::zoomOut()
{
    if ( parent != NULL )
    {
        parent->getTransform()->zoomOut();
    }
}
void
SystemTopologyToolBar::reset()
{
    if ( parent != NULL )
    {
        parent->getTransform()->rescale( true );
    }
}
void
SystemTopologyToolBar::scale()
{
    if ( parent != NULL )
    {
        parent->getTransform()->rescale( false );
    }
}

void
SystemTopologyToolBar::coloring()
{
    if ( parent != NULL )
    {
        service->setUserDefinedMinMaxValues( SYSTEM );
        parent->valuesChanged();
    }
}

// the topology tool bar's actions are enabled only if in the tab widget
// the current element is a topology display (SystemSystemTopologyToolBar)

void
SystemTopologyToolBar::enableTopologyButtons()
{
    leftAct->setEnabled( true );
    rightAct->setEnabled( true );
    upAct->setEnabled( true );
    downAct->setEnabled( true );
    dist1Act->setEnabled( true );
    dist2Act->setEnabled( true );
    zoom1Act->setEnabled( true );
    zoom2Act->setEnabled( true );
    resetAct->setEnabled( true );
    scaleAct->setEnabled( true );
    userAct->setEnabled( true );
    xLabel->setEnabled( true );
    yLabel->setEnabled( true );
    xSpin->setEnabled( true );
    ySpin->setEnabled( true );
}

void
SystemTopologyToolBar::disableTopologyButtons()
{
    leftAct->setEnabled( false );
    rightAct->setEnabled( false );
    upAct->setEnabled( false );
    downAct->setEnabled( false );
    dist1Act->setEnabled( false );
    dist2Act->setEnabled( false );
    zoom1Act->setEnabled( false );
    zoom2Act->setEnabled( false );
    resetAct->setEnabled( false );
    scaleAct->setEnabled( false );
    userAct->setEnabled( false );
    xLabel->setEnabled( false );
    yLabel->setEnabled( false );
    xSpin->setEnabled( false );
    ySpin->setEnabled( false );
}

void
SystemTopologyToolBar::setRecentSelection( TreeItem* item )
{
    recentSelection = item;
}

TreeItem*
SystemTopologyToolBar::getRecentSelection()
{
    return recentSelection;
}
