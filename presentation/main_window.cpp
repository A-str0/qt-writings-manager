#include "presentation/main_window.hpp"

#include <QFileDialog>
#include <QHeaderView>
#include <QTableWidgetItem>

#include "ui_main_window.h"

namespace {

QString yearToText(int year)
{
    return year == 0 ? QString() : QString::number(year);
}

} // namespace

namespace Presentation {

MainWindow::MainWindow(Application::LibraryCatalogService &service, QWidget *parent)
    : QMainWindow(parent)
    , m_service(service)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    m_ui->writingsTable->horizontalHeader()->setStretchLastSection(true);
    m_ui->writingsTable->verticalHeader()->setVisible(false);

    connect(m_ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(m_ui->updateButton, &QPushButton::clicked, this, &MainWindow::onUpdateClicked);
    connect(m_ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(m_ui->loadButton, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
    connect(m_ui->saveButton, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(m_ui->clearButton, &QPushButton::clicked, this, &MainWindow::onClearClicked);
    connect(
        m_ui->writingsTable,
        &QTableWidget::itemSelectionChanged,
        this,
        &MainWindow::onTableSelectionChanged);

    refreshTable();
    updateActionButtons();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::onAddClicked()
{
    Domain::Writing writing;
    if (!buildWritingFromForm(writing)) {
        return;
    }

    const Application::OperationResult result = m_service.addWriting(writing);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    refreshTable();
    clearForm();
}

void MainWindow::onUpdateClicked()
{
    if (m_selectedTitle.isEmpty()) {
        showStatus(QStringLiteral("Сначала выберите запись для редактирования."));
        return;
    }

    Domain::Writing writing;
    if (!buildWritingFromForm(writing)) {
        return;
    }

    const Application::OperationResult result =
        m_service.updateWriting(m_selectedTitle, writing);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    refreshTable();
    clearForm();
}

void MainWindow::onDeleteClicked()
{
    if (m_selectedTitle.isEmpty()) {
        showStatus(QStringLiteral("Сначала выберите запись для удаления."));
        return;
    }

    const Application::OperationResult result = m_service.removeWriting(m_selectedTitle);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    refreshTable();
    clearForm();
}

void MainWindow::onLoadClicked()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Загрузить каталог из CSV"),
        {},
        QStringLiteral("CSV files (*.csv);;All files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    const Application::OperationResult result = m_service.loadFromFile(filePath);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    refreshTable();
    clearForm();
}

void MainWindow::onSaveClicked()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("Сохранить каталог в CSV"),
        {},
        QStringLiteral("CSV files (*.csv);;All files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath += ".csv";
    }

    const Application::OperationResult result = m_service.saveToFile(filePath);
    showStatus(result.message);
}

void MainWindow::onClearClicked()
{
    clearForm();
    showStatus({});
}

void MainWindow::onTableSelectionChanged()
{
    const int row = m_ui->writingsTable->currentRow();
    if (row < 0) {
        m_selectedTitle.clear();
        updateActionButtons();
        return;
    }

    const QString title = m_ui->writingsTable->item(row, 0)->text();
    const QString author = m_ui->writingsTable->item(row, 1)->text();
    const QString genre = m_ui->writingsTable->item(row, 2)->text();
    const QString yearText = m_ui->writingsTable->item(row, 3)->text();
    const QString description = m_ui->writingsTable->item(row, 4)->text();

    bool ok = false;
    const int year = yearText.toInt(&ok);

    m_selectedTitle = title;
    fillForm(Domain::Writing(title, author, genre, ok ? year : 0, description));
    updateActionButtons();
}

void MainWindow::refreshTable()
{
    const QList<Domain::Writing> writings = m_service.listWritings();

    m_ui->writingsTable->setRowCount(writings.size());

    for (int row = 0; row < writings.size(); ++row) {
        const Domain::Writing &writing = writings.at(row);

        m_ui->writingsTable->setItem(row, 0, new QTableWidgetItem(writing.title()));
        m_ui->writingsTable->setItem(row, 1, new QTableWidgetItem(writing.author()));
        m_ui->writingsTable->setItem(row, 2, new QTableWidgetItem(writing.genre()));
        m_ui->writingsTable->setItem(
            row, 3, new QTableWidgetItem(yearToText(writing.publicationYear())));
        m_ui->writingsTable->setItem(row, 4, new QTableWidgetItem(writing.description()));
    }

    m_ui->writingsTable->resizeColumnsToContents();
}

void MainWindow::clearForm()
{
    m_selectedTitle.clear();
    m_ui->titleEdit->clear();
    m_ui->authorEdit->clear();
    m_ui->genreEdit->clear();
    m_ui->yearEdit->clear();
    m_ui->descriptionEdit->clear();
    m_ui->writingsTable->clearSelection();
    updateActionButtons();
}

void MainWindow::fillForm(const Domain::Writing &writing)
{
    m_ui->titleEdit->setText(writing.title());
    m_ui->authorEdit->setText(writing.author());
    m_ui->genreEdit->setText(writing.genre());
    m_ui->yearEdit->setText(yearToText(writing.publicationYear()));
    m_ui->descriptionEdit->setPlainText(writing.description());
}

bool MainWindow::buildWritingFromForm(Domain::Writing &writing)
{
    int year = 0;
    const QString yearText = m_ui->yearEdit->text().trimmed();

    if (!yearText.isEmpty()) {
        bool ok = false;
        year = yearText.toInt(&ok);

        if (!ok) {
            showStatus(QStringLiteral("Год издания должен быть целым числом."));
            return false;
        }
    }

    writing = Domain::Writing(
        m_ui->titleEdit->text(),
        m_ui->authorEdit->text(),
        m_ui->genreEdit->text(),
        year,
        m_ui->descriptionEdit->toPlainText());
    return true;
}

void MainWindow::showStatus(const QString &message)
{
    m_ui->statusLabel->setText(message);
}

void MainWindow::updateActionButtons()
{
    const bool hasSelection = !m_selectedTitle.isEmpty();
    m_ui->updateButton->setEnabled(hasSelection);
    m_ui->deleteButton->setEnabled(hasSelection);
}

} // namespace Presentation
