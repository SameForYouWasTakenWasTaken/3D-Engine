#include <App/App.hpp>

int main()
{
    AppSettings settings; // On default settings already.
    App app(settings);

    app.Run();

    
    return 0;
}