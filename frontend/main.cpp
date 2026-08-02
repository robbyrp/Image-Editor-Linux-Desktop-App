#include "application.h"

int main()
{
	Ui::Application app;
	if (!app.init(1280, 720, "Image Editor")) return 1;

	app.run();

	app.shutdown();

	return 0;
}
