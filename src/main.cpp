#include <QApplication>
#include <QPushButton>

#include <algorithm>
#include <iterator>
#include <string_view>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    if ( auto end = argv + argc; end != std::find_if(argv, end, [](std::string_view argument) {return argument == "--dryrun"; }))
    {
        app.quit();
    }

    QPushButton button("Hello world !");
    button.show();

    return app.exec();
}