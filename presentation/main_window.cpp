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

MainWindow::MainWindow(
    Application::AuthorsService &authorsService,
    Application::WritingsService &writingsService,
    QWidget *parent)
    : QMainWindow(parent)
    , m_authorsService(authorsService)
    , m_writingsService(writingsService)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    m_ui->authorsTable->horizontalHeader()->setStretchLastSection(true);
    m_ui->authorsTable->verticalHeader()->setVisible(false);
    m_ui->writingsTable->horizontalHeader()->setStretchLastSection(true);
    m_ui->writingsTable->verticalHeader()->setVisible(false);

    connect(m_ui->authorsLoadButton, &QPushButton::clicked, this, &MainWindow::onLoadAuthorsClicked);
    connect(m_ui->authorsSaveButton, &QPushButton::clicked, this, &MainWindow::onSaveAuthorsClicked);
    connect(m_ui->authorAddButton, &QPushButton::clicked, this, &MainWindow::onAddAuthorClicked);
    connect(
        m_ui->authorUpdateButton, &QPushButton::clicked, this, &MainWindow::onUpdateAuthorClicked);
    connect(
        m_ui->authorDeleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteAuthorClicked);
    connect(m_ui->authorClearButton, &QPushButton::clicked, this, &MainWindow::onClearAuthorClicked);

    connect(
        m_ui->writingsLoadButton, &QPushButton::clicked, this, &MainWindow::onLoadWritingsClicked);
    connect(
        m_ui->writingsSaveButton, &QPushButton::clicked, this, &MainWindow::onSaveWritingsClicked);
    connect(m_ui->writingAddButton, &QPushButton::clicked, this, &MainWindow::onAddWritingClicked);
    connect(
        m_ui->writingUpdateButton,
        &QPushButton::clicked,
        this,
        &MainWindow::onUpdateWritingClicked);
    connect(
        m_ui->writingDeleteButton,
        &QPushButton::clicked,
        this,
        &MainWindow::onDeleteWritingClicked);
    connect(
        m_ui->writingClearButton,
        &QPushButton::clicked,
        this,
        &MainWindow::onClearWritingClicked);

    connect(
        m_ui->authorsTable,
        &QTableWidget::itemSelectionChanged,
        this,
        &MainWindow::onAuthorTableSelectionChanged);
    connect(
        m_ui->writingsTable,
        &QTableWidget::itemSelectionChanged,
        this,
        &MainWindow::onWritingTableSelectionChanged);

    refreshAuthorsTable();
    refreshAuthorCombo();
    refreshWritingsTable();
    updateActionButtons();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::onLoadAuthorsClicked()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Загрузить авторов из CSV"),
        QString(),
        QStringLiteral("CSV files (*.csv);;All files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    const Application::OperationResult result = m_authorsService.loadFromFile(filePath);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    clearAuthorForm();
    refreshAuthorsTable();
    refreshAuthorCombo();
    refreshWritingsTable();
}

void MainWindow::onSaveAuthorsClicked()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("Сохранить авторов в CSV"),
        QString(),
        QStringLiteral("CSV files (*.csv);;All files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath += ".csv";
    }

    const Application::OperationResult result = m_authorsService.saveToFile(filePath);
    showStatus(result.message);
}

void MainWindow::onAddAuthorClicked()
{
    Domain::Author author;
    if (!buildAuthorFromForm(author)) {
        return;
    }

    const Application::OperationResult result = m_authorsService.addAuthor(author);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    refreshAuthorsTable();
    refreshAuthorCombo();
    clearAuthorForm();
}

void MainWindow::onUpdateAuthorClicked()
{
    if (m_selectedAuthorId.isEmpty()) {
        showStatus(QStringLiteral("Сначала выберите автора для редактирования."));
        return;
    }

    Domain::Author author;
    if (!buildAuthorFromForm(author)) {
        return;
    }

    const Application::OperationResult result =
        m_authorsService.updateAuthor(m_selectedAuthorId, author);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    const QString selectedAuthorId = m_selectedAuthorId;
    refreshAuthorsTable();
    refreshAuthorCombo(selectedAuthorId);
    refreshWritingsTable();
    clearAuthorForm();
}

void MainWindow::onDeleteAuthorClicked()
{
    if (m_selectedAuthorId.isEmpty()) {
        showStatus(QStringLiteral("Сначала выберите автора для удаления."));
        return;
    }

    const Application::OperationResult result = m_authorsService.removeAuthor(m_selectedAuthorId);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    refreshAuthorsTable();
    refreshAuthorCombo();
    clearAuthorForm();
}

void MainWindow::onClearAuthorClicked()
{
    clearAuthorForm();
    showStatus(QString());
}

void MainWindow::onLoadWritingsClicked()
{
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Загрузить произведения из CSV"),
        QString(),
        QStringLiteral("CSV files (*.csv);;All files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    const Application::OperationResult result = m_writingsService.loadFromFile(filePath);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    clearWritingForm();
    refreshWritingsTable();
}

void MainWindow::onSaveWritingsClicked()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        QStringLiteral("Сохранить произведения в CSV"),
        QString(),
        QStringLiteral("CSV files (*.csv);;All files (*)"));

    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        filePath += ".csv";
    }

    const Application::OperationResult result = m_writingsService.saveToFile(filePath);
    showStatus(result.message);
}

void MainWindow::onAddWritingClicked()
{
    Domain::Writing writing;
    if (!buildWritingFromForm(writing)) {
        return;
    }

    const Application::OperationResult result = m_writingsService.addWriting(writing);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    refreshWritingsTable();
    clearWritingForm();
}

void MainWindow::onUpdateWritingClicked()
{
    if (m_selectedWritingId.isEmpty()) {
        showStatus(QStringLiteral("Сначала выберите произведение для редактирования."));
        return;
    }

    Domain::Writing writing;
    if (!buildWritingFromForm(writing)) {
        return;
    }

    const Application::OperationResult result =
        m_writingsService.updateWriting(m_selectedWritingId, writing);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    refreshWritingsTable();
    clearWritingForm();
}

void MainWindow::onDeleteWritingClicked()
{
    if (m_selectedWritingId.isEmpty()) {
        showStatus(QStringLiteral("Сначала выберите произведение для удаления."));
        return;
    }

    const Application::OperationResult result = m_writingsService.removeWriting(m_selectedWritingId);
    showStatus(result.message);

    if (!result.ok) {
        return;
    }

    refreshWritingsTable();
    clearWritingForm();
}

void MainWindow::onClearWritingClicked()
{
    clearWritingForm();
    showStatus(QString());
}

void MainWindow::onAuthorTableSelectionChanged()
{
    const int row = m_ui->authorsTable->currentRow();
    if (row < 0) {
        m_selectedAuthorId.clear();
        updateActionButtons();
        return;
    }

    QTableWidgetItem *nameItem = m_ui->authorsTable->item(row, 0);
    if (!nameItem) {
        m_selectedAuthorId.clear();
        updateActionButtons();
        return;
    }

    m_selectedAuthorId = nameItem->data(Qt::UserRole).toString();

    bool yearOk = false;
    const int birthYear = m_ui->authorsTable->item(row, 1)->text().toInt(&yearOk);

    fillAuthorForm(Domain::Author(
        m_selectedAuthorId,
        nameItem->text(),
        yearOk ? birthYear : 0,
        m_ui->authorsTable->item(row, 2)->text()));
    updateActionButtons();
}

void MainWindow::onWritingTableSelectionChanged()
{
    const int row = m_ui->writingsTable->currentRow();
    if (row < 0) {
        m_selectedWritingId.clear();
        updateActionButtons();
        return;
    }

    QTableWidgetItem *titleItem = m_ui->writingsTable->item(row, 0);
    if (!titleItem) {
        m_selectedWritingId.clear();
        updateActionButtons();
        return;
    }

    m_selectedWritingId = titleItem->data(Qt::UserRole).toString();
    const QString authorId = titleItem->data(Qt::UserRole + 1).toString();

    bool yearOk = false;
    const int publicationYear = m_ui->writingsTable->item(row, 3)->text().toInt(&yearOk);

    fillWritingForm(Domain::Writing(
        m_selectedWritingId,
        titleItem->text(),
        authorId,
        m_ui->writingsTable->item(row, 2)->text(),
        yearOk ? publicationYear : 0,
        m_ui->writingsTable->item(row, 4)->text()));
    updateActionButtons();
}

void MainWindow::refreshAuthorsTable()
{
    const QList<Domain::Author> authors = m_authorsService.listAuthors();
    m_ui->authorsTable->setRowCount(authors.size());

    for (int row = 0; row < authors.size(); ++row) {
        const Domain::Author &author = authors.at(row);

        auto *nameItem = new QTableWidgetItem(author.name());
        nameItem->setData(Qt::UserRole, author.id());

        m_ui->authorsTable->setItem(row, 0, nameItem);
        m_ui->authorsTable->setItem(row, 1, new QTableWidgetItem(yearToText(author.birthYear())));
        m_ui->authorsTable->setItem(row, 2, new QTableWidgetItem(author.country()));
    }

    m_ui->authorsTable->resizeColumnsToContents();
}

void MainWindow::refreshWritingsTable()
{
    const QList<Domain::Writing> writings = m_writingsService.listWritings();
    m_ui->writingsTable->setRowCount(writings.size());

    for (int row = 0; row < writings.size(); ++row) {
        const Domain::Writing &writing = writings.at(row);

        auto *titleItem = new QTableWidgetItem(writing.title());
        titleItem->setData(Qt::UserRole, writing.id());
        titleItem->setData(Qt::UserRole + 1, writing.authorId());

        m_ui->writingsTable->setItem(row, 0, titleItem);
        m_ui->writingsTable->setItem(row, 1, new QTableWidgetItem(authorNameById(writing.authorId())));
        m_ui->writingsTable->setItem(row, 2, new QTableWidgetItem(writing.genre()));
        m_ui->writingsTable->setItem(
            row, 3, new QTableWidgetItem(yearToText(writing.publicationYear())));
        m_ui->writingsTable->setItem(row, 4, new QTableWidgetItem(writing.description()));
    }

    m_ui->writingsTable->resizeColumnsToContents();
}

void MainWindow::refreshAuthorCombo(const QString &preferredAuthorId)
{
    QString authorIdToSelect = preferredAuthorId;
    if (authorIdToSelect.isEmpty()) {
        authorIdToSelect = m_ui->writingAuthorCombo->currentData().toString();
    }

    const QList<Domain::Author> authors = m_authorsService.listAuthors();

    m_ui->writingAuthorCombo->clear();

    for (const Domain::Author &author : authors) {
        m_ui->writingAuthorCombo->addItem(author.name(), author.id());
    }

    if (authorIdToSelect.isEmpty()) {
        updateActionButtons();
        return;
    }

    const int index = m_ui->writingAuthorCombo->findData(authorIdToSelect);
    if (index >= 0) {
        m_ui->writingAuthorCombo->setCurrentIndex(index);
    }

    updateActionButtons();
}

void MainWindow::clearAuthorForm()
{
    m_selectedAuthorId.clear();
    m_ui->authorNameEdit->clear();
    m_ui->authorBirthYearEdit->clear();
    m_ui->authorCountryEdit->clear();
    m_ui->authorsTable->clearSelection();
    updateActionButtons();
}

void MainWindow::clearWritingForm()
{
    m_selectedWritingId.clear();
    m_ui->writingTitleEdit->clear();
    m_ui->writingGenreEdit->clear();
    m_ui->writingYearEdit->clear();
    m_ui->writingDescriptionEdit->clear();
    m_ui->writingsTable->clearSelection();
    updateActionButtons();
}

void MainWindow::fillAuthorForm(const Domain::Author &author)
{
    m_ui->authorNameEdit->setText(author.name());
    m_ui->authorBirthYearEdit->setText(yearToText(author.birthYear()));
    m_ui->authorCountryEdit->setText(author.country());
}

void MainWindow::fillWritingForm(const Domain::Writing &writing)
{
    m_ui->writingTitleEdit->setText(writing.title());
    m_ui->writingGenreEdit->setText(writing.genre());
    m_ui->writingYearEdit->setText(yearToText(writing.publicationYear()));
    m_ui->writingDescriptionEdit->setPlainText(writing.description());

    const int authorIndex = m_ui->writingAuthorCombo->findData(writing.authorId());
    if (authorIndex >= 0) {
        m_ui->writingAuthorCombo->setCurrentIndex(authorIndex);
    }
}

bool MainWindow::buildAuthorFromForm(Domain::Author &author)
{
    int birthYear = 0;
    const QString yearText = m_ui->authorBirthYearEdit->text().trimmed();

    if (!yearText.isEmpty()) {
        bool ok = false;
        birthYear = yearText.toInt(&ok);

        if (!ok) {
            showStatus(QStringLiteral("Год рождения автора должен быть целым числом."));
            return false;
        }
    }

    author = Domain::Author(
        QString(), m_ui->authorNameEdit->text(), birthYear, m_ui->authorCountryEdit->text());
    return true;
}

bool MainWindow::buildWritingFromForm(Domain::Writing &writing)
{
    const QString authorId = m_ui->writingAuthorCombo->currentData().toString();
    if (authorId.isEmpty()) {
        showStatus(QStringLiteral("Сначала добавьте автора и выберите его для произведения."));
        return false;
    }

    int publicationYear = 0;
    const QString yearText = m_ui->writingYearEdit->text().trimmed();

    if (!yearText.isEmpty()) {
        bool ok = false;
        publicationYear = yearText.toInt(&ok);

        if (!ok) {
            showStatus(QStringLiteral("Год издания должен быть целым числом."));
            return false;
        }
    }

    writing = Domain::Writing(
        QString(),
        m_ui->writingTitleEdit->text(),
        authorId,
        m_ui->writingGenreEdit->text(),
        publicationYear,
        m_ui->writingDescriptionEdit->toPlainText());
    return true;
}

QString MainWindow::authorNameById(const QString &authorId) const
{
    for (const Domain::Author &author : m_authorsService.listAuthors()) {
        if (author.id() == authorId) {
            return author.name();
        }
    }

    return QStringLiteral("—");
}

void MainWindow::showStatus(const QString &message)
{
    m_ui->statusLabel->setText(message);
}

void MainWindow::updateActionButtons()
{
    const bool hasAuthorSelection = !m_selectedAuthorId.isEmpty();
    const bool hasWritingSelection = !m_selectedWritingId.isEmpty();
    const bool hasAnyAuthors = m_ui->writingAuthorCombo->count() > 0;

    m_ui->authorUpdateButton->setEnabled(hasAuthorSelection);
    m_ui->authorDeleteButton->setEnabled(hasAuthorSelection);
    m_ui->writingAddButton->setEnabled(hasAnyAuthors);
    m_ui->writingUpdateButton->setEnabled(hasWritingSelection);
    m_ui->writingDeleteButton->setEnabled(hasWritingSelection);
}

} // namespace Presentation
