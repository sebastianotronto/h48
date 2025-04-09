/*
C++20 interface for nissy.

TODO: add more documentation (here and in README.md)
*/

#include "./nissy.h"

extern "C" {
	long long nissy_compose(const char *, const char *, char *);
	long long nissy_inverse(const char *, char *);
	long long nissy_applymoves(const char *, const char *, char *);
	long long nissy_applytrans(const char *, const char *, char *);
	long long nissy_convert(const char *, const char *, const char *,
	    unsigned, char *);
	long long nissy_getcube(long long, long long, long long, long long,
	    const char *, char *);
	long long nissy_solverinfo(const char *, char *);
	long long nissy_gendata(const char *, unsigned long long, char *);
	long long nissy_checkdata(unsigned long long, const char *);
	long long nissy_solve(const char *, const char *, unsigned, unsigned,
	    unsigned, unsigned, int, int, unsigned long long, const char *,
	    unsigned, char *, long long *);
	long long nissy_countmoves(const char *);
	long long nissy_setlogger(void (*)(const char *));
}

namespace nissy {
	namespace utils {
		void fixstr(std::string& str)
		{
			size_t n = str.find('\0');
			str.resize(n);
		}

		std::variant<cube_t, error_t>
		    cube_or_error(const char *r, long long e)
		{
			if (e < 0)
				return error_t{e};
			return cube_t{r};
		}

		std::variant<std::string, error_t>
		    string_or_error(const char *cstr, long long e)
		{
			if (e < 0)
				return error_t{e};

			std::string str{cstr};
			fixstr(str);
			return str;
		}

		std::function<void(const char *)>
		    char_str_fn(std::function<void(std::string&)> f)
		{
			return [&](const char *cc){
				std::string str{cc};
				f(str);
			};
		}
	}
	
	std::variant<cube_t, error_t>
	compose(const cube_t& cube, const cube_t& permutation)
	{
		char result[size::B32];

		auto error = nissy_compose(cube.value.c_str(),
		    permutation.value.c_str(), result);

		return utils::cube_or_error(result, error);
	}

	std::variant<cube_t, error_t>
	inverse(const cube_t& cube)
	{
		char result[size::B32];

		auto error = nissy_inverse(cube.value.c_str(), result);

		return utils::cube_or_error(result, error);
	}

	std::variant<cube_t, error_t>
	applymoves(const cube_t& cube, const moves_t& moves)
	{
		char result[size::B32];

		auto error = nissy_applymoves(cube.value.c_str(),
		    moves.value.c_str(), result);

		return utils::cube_or_error(result, error);
	}

	std::variant<cube_t, error_t>
	applytrans(const cube_t& cube, const trans_t& trans)
	{
		char result[size::B32];

		auto error = nissy_applytrans(cube.value.c_str(),
		    trans.value.c_str(), result);

		return utils::cube_or_error(result, error);
	}

	std::variant<std::string, error_t>
	convert(const cube_format_t& format_in,
	    const cube_format_t& format_out, const std::string& cube_string)
	{
		char result[size::CUBE_MAX];

		auto error = nissy_convert(format_in.value.c_str(),
		    format_out.value.c_str(), cube_string.c_str(),
		    size::CUBE_MAX, result);

		return utils::string_or_error(result, error);
	}

	std::variant<cube_t, error_t>
	getcube(long long ep, long long eo, long long cp, long long co,
	    const std::string& options)
	{
		char result[size::B32];

		auto error = nissy_getcube(ep, eo, cp, co, options.c_str(),
		    result);

		return utils::cube_or_error(result, error);
	}

	std::variant<std::pair<size_t, std::string>, error_t>
	solverinfo(const solver_t& solver)
	{
		char dataid[size::DATAID];

		auto error = nissy_solverinfo(solver.value.c_str(), dataid);
		if (error < 0)
			return error_t{error};

		return std::pair{error, dataid};
	}

	std::variant<solver_data_t, error_t>
	gendata(const solver_t& solver)
	{
		char dataid[size::DATAID];

		auto error = nissy_solverinfo(solver.value.c_str(), dataid);
		if (error < 0)
			return error_t{error};

		size_t sz = error;
		std::vector<std::byte> buffer(sz);
		error = nissy_gendata(solver.value.c_str(), sz,
		    reinterpret_cast<char *>(buffer.data()));
		if (error < 0)
			return error_t{error};

		return solver_data_t{buffer};
	}

	std::optional<error_t>
	checkdata(const solver_data_t& data)
	{
		auto error = nissy_checkdata(data.value.size(),
		    reinterpret_cast<const char *>(data.value.data()));

		if (error < 0)
			return error_t{error};
		return std::nullopt;
	}

	std::variant<solve_result_t, error_t>
	solve(const cube_t& cube, const solver_t& solver,
	    nissflag_t niss, unsigned minmoves, unsigned maxmoves,
	    unsigned maxsolutions, int optimal, int threads,
	    const solver_data_t& data)
	{
		const size_t len = 3 * (maxmoves+1) * maxsolutions;
		char csols[len];
		long long cstats[size::SOLVE_STATS];

		auto error = nissy_solve(cube.value.c_str(),
		    solver.value.c_str(), niss.value, minmoves, maxmoves,
		    maxsolutions, optimal, threads, data.value.size(),
		    reinterpret_cast<const char *>(data.value.data()),
		    len, csols, cstats);

		if (error < 0)
			return error_t{error};

		std::vector<std::string> sols;
		std::string_view strsols(csols);
		for (auto r : strsols | std::views::split('\n'))
			sols.push_back(std::string{r.begin(), r.end()});

		return solve_result_t{sols, std::to_array(cstats)};
	}

	std::variant<unsigned, error_t>
	countmoves(const moves_t& moves)
	{
		auto error = nissy_countmoves(moves.value.c_str());
		if (error < 0)
			return error_t{error};
		return (unsigned)error;
	}

	void setlogger(std::function<void(std::string&)> log)
	{
		nissy_setlogger(
		    utils::char_str_fn(log).target<void(const char *)>());
	}
}
