#ifndef PRESENTATION_MAIN_WINDOW_HPP
#define PRESENTATION_MAIN_WINDOW_HPP

#include <QMainWindow>

#include "application/library_catalog_service.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace Presentation {

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Application::LibraryCatalogService &service, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onAddClicked();
    void onUpdateClicked();
    void onDeleteClicked();
    void onLoadClicked();
    void onSaveClicked();
    void onClearClicked();
    void onTableSelectionChanged();

private:
    void refreshTable();
    void clearForm();
    void fillForm(const Domain::Writing &writing);
    bool buildWritingFromForm(Domain::Writing &writing);
    void showStatus(const QString &message);
    void updateActionButtons();

    Application::LibraryCatalogService &m_service;
    Ui::MainWindow *m_ui;
    QString m_selectedTitle;
};

} // namespace Presentation

#endif // PRESENTATION_MAIN_WINDOW_HPP
