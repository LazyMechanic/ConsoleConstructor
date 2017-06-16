Usage:
```c++
#include "ConsoleConstructor.h"

using console = mech::ConsoleConstructor;

int main(int argc, char* argv[]) {
	console::setProgramName("MyProgram");
	console::on("command1", "-c1", "--command1", mech::Count::ANY, "Comment number 1");
	console::on("command2", "-c2", "--command2", "Comment number 2");
	console::on("command3", "-c3", 10, "Comment number 3");
	console::on("command3", "-c3", "Comment number 3");

	int res = console::consoleHandler(argc, argv);

	if (res == console::STATE::HELP) {
		return 0;
	}
	else if (res == console::STATE::WRONG_NUMBER_PARAMS) {
		return -1;
	}
	else if (res == console::STATE::WRONG_ARGUMENTS) {
		return -2;
	}

	std::string command1_params;

	if (console::getStatusInstruction("command1")) {
		command1_params = console::getArguments("command1")[0];
	}
	// Do smth...
}
```