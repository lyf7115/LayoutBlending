#include "LayoutExplorer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	QApplication a(argc, argv);
	LayoutExplorer w;
	w.show();
	return a.exec();
}
