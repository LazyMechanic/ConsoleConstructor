#include "ConsoleConstructor.h"
#include <iostream>
#include <iomanip>

std::vector<mech::Instruction> mech::ConsoleConstructor::m_instructions;
std::string mech::ConsoleConstructor::m_programName;

#ifdef _WIN32
#include <Windows.h>
enum COLOR {
	BLACK = 0,
	RED = FOREGROUND_RED,
	GREEN = FOREGROUND_GREEN,
	BLUE = FOREGROUND_BLUE,
	WHITE = RED | GREEN | BLUE,
	YELLOW = RED | GREEN,
	PINK = RED | BLUE,
	TURQUOISE = GREEN | BLUE,

	LIGHTER = FOREGROUND_INTENSITY,

	BLACK_BG = 0,
	RED_BG = BACKGROUND_RED,
	GREEN_BG = BACKGROUND_GREEN,
	BLUE_BG = BACKGROUND_BLUE,
	WHITE_BG = RED_BG | GREEN_BG | BLUE_BG,
	YELLOW_BG = RED_BG | GREEN_BG,
	PINK_BG = RED_BG | BLUE_BG,
	TURQUOISE_BG = GREEN_BG | BLUE_BG,

	LIGHTER_BG = BACKGROUND_INTENSITY
};
enum STATE_COLOR {
	NORMAL_C = COLOR::WHITE | COLOR::BLACK_BG,
	INVALID_NUMBER_C = COLOR::RED | COLOR::YELLOW_BG,
	WRONG_ARGUMENTS_C = COLOR::WHITE | COLOR::RED_BG
};

const HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

unsigned int GetBufferWidth()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

#elif __linux__
#include <sys/ioctl.h>
#include <unistd.h>
#include <string>

#define DEFAULT "\033[0m" // - все атрибуты по умолчанию;

#define BLACK "\033[30"; // - черный цвет знаков;
#define RED "\033[31"; // - красный цвет знаков;
#define GREEN "\033[32"; // - зеленый цвет знаков;
#define BLUE "\033[34"; // - синий цвет знаков;
#define WHITE "\033[37"; // - серый цвет знаков;
#define YELLOW "\033[33"; // - коричневый цвет знаков;
#define PINK "\033[35"; // - фиолетовый цвет знаков;
#define TURQUOISE "\033[36"; // - цвет морской волны знаков;

#define BLACK_BG "\033[40"; // - черный цвет фона;
#define RED_BG "\033[41"; // - красный цвет фона;
#define GREEN_BG "\033[42"; // - зеленый цвет фона;
#define BLUE_BG "\033[44"; // - синий цвет фона;
#define WHITE_BG "\033[47"; // - серый цвет фона.
#define YELLOW_BG "\033[43"; // - коричневый цвет фона;
#define PINK_BG "\033[45"; // - фиолетовый цвет фона;
#define TURQUOISE_BG "\033[46"; // - цвет морской волны фона;

unsigned int GetBufferWidth()
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}
#endif

std::string generateComment(std::string str) {
	size_t bufferWidth = GetBufferWidth() - 16;

	str.insert(0, "\t\t");

	size_t position = 0;
	while (position < str.size()) {
		if (position + bufferWidth >= str.size()) {
			break;
		}
		position += bufferWidth;

		size_t i = position;
		while (str[i] != ' ') {
			i--;
			if (position - i > bufferWidth) {
				i = position;
				break;
			}
		}
		str.insert(i + 1, "\n\t\t");
		position += 2;
	}
	return str;
}

void mech::ConsoleConstructor::onProgramName(const std::string & name)
{
	m_programName = name;
}

int mech::ConsoleConstructor::on(const std::string & meaning, const std::string & instruction, const std::string & altInstruction, const std::string & comment)
{
	for (int i = 0; i < m_instructions.size(); i++) {
		if (m_instructions[i].m_instruction == instruction ||
			m_instructions[i].m_altInstruction == altInstruction ||
			m_instructions[i].m_meaning == meaning) {
			return -1;
		}
	}

	m_instructions.push_back(Instruction(meaning, instruction, altInstruction, generateComment(comment)));
	return 1;
}

int mech::ConsoleConstructor::on(const std::string & instruction, const std::string& meaning, const std::string& comment)
{
	for (int i = 0; i < m_instructions.size(); i++) {
		if (m_instructions[i].m_instruction == instruction ||
			m_instructions[i].m_meaning == meaning) {
			return -1;
		}
	}

	m_instructions.push_back(Instruction(meaning, instruction, generateComment(comment)));
	return 1;
}

int mech::ConsoleConstructor::findInstruction(const std::string& instruction)
{
	for (int i = 0; i < m_instructions.size(); i++) {
		if (m_instructions[i].m_instruction == instruction ||
			m_instructions[i].m_altInstruction == instruction) {
			return i;
		}
	}
	return -1;
}

int mech::ConsoleConstructor::consoleHandler(int argc, char* argv[])
{
	for (int i = 1; i < argc; i++) {
		int position = argc;
		if (std::string(argv[i]) == "-h" || std::string(argv[i]) == "--help") {
			help(STATE::NORMAL);
			return STATE::NORMAL;
		}

		if (findInstruction(argv[i]) > -1)
		{
			m_instructions[findInstruction(argv[i])].m_status = true;
			for (int k = i + 1; k < argc; k++) {
				if (findInstruction(argv[k]) > -1) {
					position = k;
					break;
				}
			}
			for (int k = i + 1; k < position; k++) {
				m_instructions[findInstruction(argv[i])].m_arguments.push_back(argv[k]);
			}
			i = position;
		}
		else {
			help(STATE::WRONG_ARGUMENTS);
			return STATE::WRONG_ARGUMENTS;
		}
	}

	return STATE::NORMAL;
}

std::vector<std::string> mech::ConsoleConstructor::getArguments(const std::string & str)
{
	for (int i = 0; i < m_instructions.size(); i++) {
		if (m_instructions[i].m_instruction == str ||
			m_instructions[i].m_altInstruction == str ||
			m_instructions[i].m_meaning == str) {
			return m_instructions[i].m_arguments;
		}
	}
	return std::vector<std::string>();
}

bool mech::ConsoleConstructor::getStatusInstruction(const std::string & str)
{
	for (int i = 0; i < m_instructions.size(); i++) {
		if (m_instructions[i].m_instruction == str ||
			m_instructions[i].m_altInstruction == str ||
			m_instructions[i].m_meaning == str) {
			return m_instructions[i].m_status;
		}
	}
	return false;
}

void mech::ConsoleConstructor::help(STATE state)
{
	switch (state) {
		case STATE::NORMAL: {
			#ifdef _WIN32
			SetConsoleTextAttribute(console, STATE_COLOR::NORMAL_C);
			#endif
			getHelpText();
			break;
		}
		case STATE::INVALID_NUMBER:
			#ifdef _WIN32
			SetConsoleTextAttribute(console, STATE_COLOR::INVALID_NUMBER_C);
			std::cout << "\nERROR: Invalid number of arguments!\n\n";
			SetConsoleTextAttribute(console, STATE_COLOR::NORMAL_C);
			#elif __linux__
			std::cout << RED << YELLOW_BG << "\nERROR: Invalid number of arguments!\n\n" << DEFAULT;
			#endif
			getHelpText();
			break;
		case STATE::WRONG_ARGUMENTS:
			#ifdef _WIN32
			SetConsoleTextAttribute(console, STATE_COLOR::WRONG_ARGUMENTS_C);
			std::cout << "\nERROR: Wrong arguments!\n\n";
			SetConsoleTextAttribute(console, STATE_COLOR::NORMAL_C);
			#elif __linux__
			std::cout << WHITE << RED_BG << "\nERROR: Wrong arguments!\n\n" << DEFAULT;
			#endif
			getHelpText();
			break;
	}
}

void mech::ConsoleConstructor::getHelpText()
{
	std::cout << "ConsoleConstructor by Mechanic.\n\n" <<
		"Usage:\n\t" << (m_programName == "" ? "YourProgramName" : m_programName) << ".exe" << " -option parametrs\n\n" <<
		"Options:\n";
	for (int i = 0; i < m_instructions.size(); i++) {
		std::cout << "\t[" << m_instructions[i].m_instruction << '=' << m_instructions[i].m_meaning << "]\n";
		std::cout << m_instructions[i].m_comment << "\n\n";
	}
}
