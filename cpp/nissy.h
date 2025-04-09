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
	/* Some constants for size for I/O buffers */
	namespace size {
		constexpr size_t B32 = 22;
		constexpr size_t H48 = 88;
		constexpr size_t CUBE_MAX = H48;
		constexpr size_t TRANSFORMATION = 12;
		constexpr size_t SOLVE_STATS = 10;
		constexpr size_t DATAID = 255;
	}

	/* Some structs definitions for better type safety */
	struct nissflag_t { unsigned value; };
	struct error_t { long long value; };
	struct cube_t { std::string value; };
	struct moves_t { std::string value; };
	struct trans_t { std::string value; };
	struct cube_format_t { std::string value; };
	struct solver_t { std::string value; };
	struct solver_data_t { std::vector<std::byte> value; };
	struct solve_result_t {
		std::vector<std::string> solutions;
		std::array<long long, size::SOLVE_STATS> stats;
	};

	/* Flags for NISS options */
	namespace nissflag {
		constexpr nissflag_t NORMAL{1};
		constexpr nissflag_t INVERSE{2};
		constexpr nissflag_t MIXED{4};
		constexpr nissflag_t LINEAR{NORMAL.value | INVERSE.value};
		constexpr nissflag_t ALL{LINEAR.value | MIXED.value};
	}

	/* Error codes */
	namespace error {
		constexpr error_t UNSOLVABLE{-1};
		constexpr error_t INVALID_CUBE{-10};
		constexpr error_t UNSOLVABLE_CUBE{-11};
		constexpr error_t INVALID_MOVES{-20};
		constexpr error_t INVALID_TRANS{-30};
		constexpr error_t INVALID_FORMAT{-40};
		constexpr error_t INVALID_SOLVER{-50};
		constexpr error_t NULL_POINTER{-60};
		constexpr error_t BUFFER_SIZE{-61};
		constexpr error_t DATA{-70};
		constexpr error_t OPTIONS{-80};
		constexpr error_t UNKNOWN{-999};
	}

	/* Cube constants */
	namespace cube {
		const cube_t SOLVED{"ABCDEFGH=ABCDEFGHIJKL"};
	}

	std::variant<cube_t, error_t> inverse(
		const cube_t& cube
	);

	std::variant<cube_t, error_t> applymoves(
		const cube_t& cube,
		const moves_t& moves
	);

	std::variant<cube_t, error_t> applytrans(
		const cube_t& cube,
		const trans_t& trans
	);

	std::variant<std::string, error_t> convert(
		const cube_format_t& format_in,
		const cube_format_t& format_out,
		const std::string& cube_string
	);

	std::variant<cube_t, error_t> getcube(
		long long ep,
		long long eo,
		long long cp,
		long long co,
		const std::string& options
	);

	std::variant<std::pair<size_t, std::string>, error_t> solverinfo(
		const solver_t& solver
	);

	std::variant<solver_data_t, error_t> gendata(
		const solver_t& solver
	);

	std::optional<error_t> checkdata(
		const solver_data_t& data
	);

	std::variant<solve_result_t, error_t> solve(
		const cube_t& cube,
		const solver_t& solver,
		nissflag_t niss,
		unsigned minmoves,
		unsigned maxmoves,
		unsigned maxsolutions,
		int optimal,
		int threads,
		const solver_data_t& data
	);

	std::variant<unsigned, error_t> countmoves(
		const moves_t& moves
	);

	void setlogger(
		std::function<void(std::string&)> log
	);
}

#endif
