#pragma once
#include <string>
#include <vector>
#include <map>



namespace mech
{
	enum Count {
		ANY = -1
	};

	struct Instruction {
	public:
		Instruction(const std::string & meaning,
			const std::string & instruction,
			int paramsCount,
			const std::string & comment) :
				m_meaning(meaning),
				m_instruction(instruction),
				m_altInstruction(std::string("")),
				m_paramsCount(paramsCount),
				m_comment(comment),
				m_status(false),
				m_arguments(std::vector<std::string>()) {}

		Instruction(const std::string & meaning,
			const std::string & instruction,
			const std::string& altInstruction,
			int paramsCount,
			const std::string & comment) :
				m_meaning(meaning),
				m_instruction(instruction),
				m_altInstruction(altInstruction),
				m_paramsCount(paramsCount),
				m_comment(comment),
				m_status(false),
				m_arguments(std::vector<std::string>()) {}

		std::string m_meaning;
		std::string m_instruction;
		std::string m_altInstruction;
		std::vector<std::string> m_arguments;
		std::string m_comment;
		int m_paramsCount;
		bool m_status;
	};

	class ConsoleConstructor {
	public:
		enum STATE : int {
			NORMAL = 1,
			HELP = 0,
			WRONG_NUMBER_PARAMS = -1,
			WRONG_ARGUMENTS = -2
		};

		static int consoleHandler(int argc, char* argv[]);

		static void setProgramName(const std::string& name);

		static int on(const std::string& meaning, const std::string& instruction, const std::string& altInstruction, int paramsCount, const std::string& comment);
		static int on(const std::string& meaning, const std::string& instruction, const std::string& altInstruction, const std::string& comment);
		static int on(const std::string& meaning, const std::string& instruction, int paramsCount, const std::string& comment);
		static int on(const std::string& meaning, const std::string& instruction, const std::string& comment);

		// str = instruction/altInstruction/meaning
		static std::vector<std::string> getArguments(const std::string& str);

		// str = instruction/altInstruction/meaning
		static bool getStatusInstruction(const std::string& str);
	private:
		static void help(STATE state = STATE::NORMAL);
		static void getHelpText();

		static std::vector<mech::Instruction> m_instructions;
		static int findInstruction(const std::string& str);

		static std::string m_programName;
	};
}