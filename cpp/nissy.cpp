/*
C++20 interface for nissy.

TODO: add more documentation (here and in README.md)
*/

#include "./nissy.h"

#include <fstream>

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
	long long nissy_setlogger(void (*)(const char *, void *), void *);
}

namespace nissy {

	/* Definition of constants with const qualifier */
	const nissflag nissflag::NORMAL{1};
	const nissflag nissflag::INVERSE{2};
	const nissflag nissflag::MIXED{4};
	const nissflag nissflag::LINEAR{3};
	const nissflag nissflag::ALL{7};

	const error error::OK{0};
	const error error::UNSOLVABLE{-1};
	const error error::INVALID_CUBE{-10};
	const error error::UNSOLVABLE_CUBE{-11};
	const error error::INVALID_MOVES{-20};
	const error error::INVALID_TRANS{-30};
	const error error::INVALID_FORMAT{-40};
	const error error::INVALID_SOLVER{-50};
	const error error::NULL_POINTER{-60};
	const error error::BUFFER_SIZE{-61};
	const error error::DATA{-70};
	const error error::OPTIONS{-80};
	const error error::UNKNOWN{-999};

	namespace size {
		constexpr size_t B32 = 22;
		constexpr size_t H48 = 88;
		constexpr size_t CUBE_MAX = H48;
		constexpr size_t TRANSFORMATION = 12;
		constexpr size_t DATAID = 255;
	}

	bool error::ok() const { return value >= 0; }

	cube::cube() {}

	error cube::move(const std::string& moves)
	{
		char result[size::B32];
		long long err = nissy_applymoves(
		    m_b32.c_str(), moves.c_str(), result);
		if (err < 0)
			return error{err};
		m_b32 = result;
		return error::OK;
	}

	error cube::transform(const std::string& trans)
	{
		char result[size::B32];
		long long err = nissy_applytrans(
		    m_b32.c_str(), trans.c_str(), result);
		if (err < 0)
			return error{err};
		m_b32 = result;
		return error::OK;
	}

	void cube::invert()
	{
		char result[size::B32];
		nissy_inverse(m_b32.c_str(), result);
		m_b32 = result;
	}

	void cube::compose(const cube& other)
	{
		char result[size::B32];
		nissy_compose(
		    m_b32.c_str(), other.to_string().c_str(), result);
		m_b32 = result;
	}

	std::string cube::to_string() const { return m_b32; }

	std::variant<std::string, error>
	cube::to_string(const std::string& format) const
	{
		char result[size::CUBE_MAX];
		auto err = nissy_convert("B32", format.c_str(),
		    m_b32.c_str(), size::CUBE_MAX, result);
		if (err < 0)
			return error{err};
		else
			return result;
	}

	std::variant<cube, error>
	cube::from_string(const std::string& str)
	{
		return from_string(str, "B32");
	}

	std::variant<cube, error>
	cube::from_string(const std::string& str, const std::string& format)
	{
		char result[size::B32];
		cube c;
		auto err = nissy_convert(format.c_str(),
		    "B32", c.m_b32.c_str(), size::B32, result);
		if (err < 0)
			return error{err};
		c.m_b32 = result;
		return c;
	}

	std::variant<cube, error>
	cube::get(long long ep, long long eo, long long cp, long long co)
	{
		return get(ep, eo, cp, co, "fix");
	}

	std::variant<cube, error>
	cube::get(long long ep, long long eo, long long cp, long long co,
	    const std::string& options)
	{
		char result[size::B32];
		cube c;
		auto err = nissy_getcube(
		    ep, eo, cp, co, options.c_str(), result);
		if (err < 0)
			return error{err};
		c.m_b32 = result;
		return c;
	}

	error solver::generate_data()
	{
		data.resize(size);
		auto err = nissy_gendata(name.c_str(),
		    size, reinterpret_cast<char *>(data.data()));
		return error{err};
	}

	void solver::read_data(std::ifstream& ifs)
	{
		data.resize(size);
		ifs.read(reinterpret_cast<char *>(data.data()), size);
	}

	error solver::check_data()
	{
		auto err_value = nissy_checkdata(data.size(),
		    reinterpret_cast<const char *>(data.data()));
		error err{err_value};
		data_checked = err.ok();
		return err;
	}

	void solver::unload_data()
	{
		data.resize(0);
	}

	solver::solve_result
	solver::solve(const cube& cube, nissflag niss, unsigned minmoves,
	    unsigned maxmoves, unsigned maxsols, int optimal, int threads)
	{
		const size_t len = 3 * (maxmoves+1) * maxsols;
		std::vector<char> csols(len);
		solver::solve_result result;

		auto err = nissy_solve(cube.to_string().c_str(),
		    name.c_str(), niss.value, minmoves, maxmoves, maxsols,
		    optimal, threads, data.size(),
		    reinterpret_cast<const char *>(data.data()), len,
		    csols.data(), result.stats.data());
		result.err = error{err};

		if (err < 0)
			return result;

		std::string_view strsols(csols.data());
		for (auto r : strsols | std::views::split('\n'))
			if (r.begin() != r.end())
				result.solutions.push_back(
				    std::string{r.begin(), r.end()});

		return result;
	}

	std::variant<solver, error> solver::get(const std::string& name)
	{
		char dataid[size::DATAID];
		auto err = nissy_solverinfo(name.c_str(), dataid);
		if (err < 0)
			return error{err};
		solver s(name);
		s.size = (unsigned)err;
		s.id = std::string{dataid};
		return s;
	}

	solver::solver(const std::string& str) : name{str} {}

	error count_moves(const std::string& moves)
	{
		auto err = nissy_countmoves(moves.c_str());
		return error{err};
	}

	void set_logger(void (*log)(const char *, void *), void *data)
	{
		nissy_setlogger(log, data);
	}
}
