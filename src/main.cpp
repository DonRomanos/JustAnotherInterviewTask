#include "ui.hpp"

#include <QApplication>
#include <QTimer>

#include <algorithm>
#include <iterator>
#include <string_view>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    
    // Here for a test to see if the application even starts, e.g. all Qt plugins are in place
    if ( auto end = argv + argc; end != std::find_if(argv, end, [](std::string_view argument) {return argument == "--dryrun"; }))
    {
        QTimer::singleShot(0, &app, SLOT(quit()));
    }

    auto main_panel = ui::MainPanel{};
    main_panel.show();

    return app.exec();
}