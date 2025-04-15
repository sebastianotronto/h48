/*
C++20 header file for nissy.
*/

#ifndef NISSY_H
#define NISSY_H

#include <array>
#include <functional>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace nissy {

	class nissflag {
	public:
		unsigned value;

		static const nissflag NORMAL;
		static const nissflag INVERSE;
		static const nissflag MIXED;
		static const nissflag LINEAR;
		static const nissflag ALL;
	};

	class error {
	public:
		long long value;
		bool ok() const;

		static const error OK;
		static const error UNSOLVABLE;
		static const error INVALID_CUBE;
		static const error UNSOLVABLE_CUBE;
		static const error INVALID_MOVES;
		static const error INVALID_TRANS;
		static const error INVALID_FORMAT;
		static const error INVALID_SOLVER;
		static const error NULL_POINTER;
		static const error BUFFER_SIZE;
		static const error DATA;
		static const error OPTIONS;
		static const error UNKNOWN;
	};

	class cube {
	public:
		cube();
		error move(const std::string&);
		error transform(const std::string&);
		void invert();
		void compose(const cube&);
		std::string to_string() const;
		std::variant<std::string, error> to_string(
		    const std::string& format) const;

		static std::variant<cube, error> from_string(
		    const std::string&);
		static std::variant<cube, error> from_string(
		    const std::string& str, const std::string& format);
		static std::variant<cube, error> get(
		    long long ep, long long eo, long long cp, long long co);
		static std::variant<cube, error> get(
		    long long ep, long long eo, long long cp, long long co,
		    const std::string& options);

	private:
		std::string m_b32{"ABCDEFGH=ABCDEFGHIJKL"};
	};

	class solver {
	public:
		struct solve_result {
			error err;
			std::vector<std::string> solutions;
			std::array<long long, 10> stats;
		};

		const std::string name;
		size_t size;
		std::string id;
		std::vector<std::byte> data;
		bool data_checked;

		error generate_data();
		void read_data(std::ifstream&);
		error check_data();
		void unload_data();
		solve_result solve(const cube&, nissflag, unsigned minmoves,
		    unsigned maxmoves, unsigned maxsols, int optimal,
		    int threads);

		static std::variant<solver, error> get(const std::string&);
	private:
		solver(const std::string& name);
	};

	error count_moves(const std::string&);
	void set_logger(const std::function<void(const char*)>&);
}

#endif
