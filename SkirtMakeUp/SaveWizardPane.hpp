/*//////////////////////////////////////////////////////////////////
////       SKIRT -- an advanced radiative transfer code         ////
////       © Astronomical Observatory, Ghent University         ////
///////////////////////////////////////////////////////////////// */

#ifndef SAVEWIZARDPANE_HPP
#define SAVEWIZARDPANE_HPP

#include <QWidget>
class SimulationItem;
class QLabel;
class QPushButton;

////////////////////////////////////////////////////////////////////

/** A SaveWizardPane instance displays the user interface for saving the current simulation item
    hierchy into a ski or fski file. */
class SaveWizardPane : public QWidget
{
    Q_OBJECT

    // ============= Construction and Destruction =============

public:
    /** The default (and only) constructor creates and initializes the GUI for this pane. The first
        argument provides a pointer to the root of the simulation item hierarchy. The subsequent
        arguments provide the filepath in which the simulation item hierarchy has been previously
        saved, if any, and the current dirty state of the hierarchy. The last argument specifies
        the object that will be notified of a successful save operation through invocation of the
        object's relevant slot. */
    explicit SaveWizardPane(SimulationItem* root, QString filepath, bool dirty, QObject* target);

    // ==================== Event Handling ====================

public slots:
    /** If the simulation item hierarchy was previously saved to a known filepath, this function
        saves the simulation item hierarchy again to the same file path, replacing the previous
        file, and notifies the target object by emitting a hierarchyWasSaved() signal. If no
        previous filepath is known, the function does nothing. */
    void save();

    /** This function displays the appropriate dialog to obtain a file path from the user, saves
        the simulation item hierarchy to the selected file path, and notifies the target object by
        emitting a hierarchyWasSaved() signal. */
    void saveAs();

    /** This function attempts to quit the application. */
    void quit();

private:
    /** This private function saves the simulation item hierarchy to the specfied file path, and
        notifies the target object by emitting a hierarchyWasSaved() signal. */
    void saveToFile(QString filepath);

    /** This function enables or disables the Save push button depending on the filename and dirty
        state, and puts the current filepath into the corresponding label. */
    void updateSaveInfo();

signals:
    /** This signal is emitted after the simulation item hierarchy has been successfully saved. */
    void hierarchyWasSaved(QString filepath);

    // ==================== Data members ======================

private:
    SimulationItem* _root;
    QString _filepath;
    bool _dirty;
    QLabel* _filepathLabel;
    QPushButton* _saveButton;
    QPushButton* _saveAsButton;
    QPushButton* _quitButton;
};

////////////////////////////////////////////////////////////////////

#endif // SAVEWIZARDPANE_HPP
