#include <QApplication>

#include "application/library_catalog_service.hpp"
#include "infrastructure/csv_writing_file_storage.hpp"
#include "infrastructure/in_memory_writings_repository.hpp"
#include "presentation/main_window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Literature Catalog");
    QApplication::setOrganizationName("course_part1");

    Infrastructure::InMemoryWritingsRepository repository;
    Infrastructure::CsvWritingFileStorage fileStorage;
    Application::LibraryCatalogService service(repository, fileStorage);
    Presentation::MainWindow window(service);
    window.show();

    return app.exec();
}
