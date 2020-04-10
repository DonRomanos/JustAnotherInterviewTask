#include <QApplication>
#include <QPushButton>
#include "qtimer.h"

#include <algorithm>
#include <iterator>
#include <string_view>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    
    // Here for a test to see if the application even starts.
    if ( auto end = argv + argc; end != std::find_if(argv, end, [](std::string_view argument) {return argument == "--dryrun"; }))
    {
        QTimer::singleShot(0, &app, SLOT(quit()));
    }

    return app.exec();
}