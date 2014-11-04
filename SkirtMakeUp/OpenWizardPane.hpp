/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef OPENWIZARDPANE_HPP
#define OPENWIZARDPANE_HPP

#include <QWidget>
class SimulationItem;
class QLabel;
class QPushButton;

////////////////////////////////////////////////////////////////////

/** An OpenWizardPane instance displays the user interface for loading a new simulation item
    hierchy from a ski or fski file. */
class OpenWizardPane : public QWidget
{
    Q_OBJECT

    // ============= Construction and Destruction =============

public:
    /** The default (and only) constructor creates and initializes the GUI for this pane. The first
        argument should be true for loading a ski file, false for loading an fski file. The second
        argument provides a filepath that has been previously used during this session, if any.
        This file path is used to position the open dialog in the file system. The third argument
        provides the dirty state of the current simulation item hierarchy, which will be
        overridden. This information is used to propertly warn the user. The last argument
        specifies the object that will be notified of a successful open and load operation through
        invocation of the object's relevant slot. */
    explicit OpenWizardPane(bool skirt, QString filepath, bool dirty, QObject* target);

    // ==================== Event Handling ====================

public slots:
    /** This function obtains an open file path from the user, and then loads the simulation item
        hierarchy from that file, replacing the previous hierarchy. After a successful load
        operation, the function notifies the target object by emitting a hierarchyWasLoaded()
        signal. */
    void open();

signals:
    /** This signal is emitted after the simulation item hierarchy has been successfully loaded. */
    void hierarchyWasLoaded(SimulationItem* root, QString filepath);

    // ==================== Data members ======================

private:
    bool _skirt;
    QString _filepath;
    bool _dirty;
    QLabel* _filepathLabel;
    QPushButton* _openButton;
};

////////////////////////////////////////////////////////////////////

#endif // OPENWIZARDPANE_HPP
