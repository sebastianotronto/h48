_static_inline cube_fast_t
cleanaftershuffle(cube_fast_t c)
{
	__m256i b;

	b = _mm256_set_epi8(
		~0, ~0, ~0, ~0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		~0, ~0, ~0, ~0, ~0, ~0, ~0, ~0, 0, 0, 0, 0, 0, 0, 0, 0
	);

	return _mm256_andnot_si256(b, c);
}

_static_inline cube_fast_t
inverse_fast(cube_fast_t c)
{
	/* Method taken from Andrew Skalski's vcube[1]. The addition sequence
	 * was generated using [2].
	 * [1] https://github.com/Voltara/vcube
	 * [2] http://wwwhomes.uni-bielefeld.de/achim/addition_chain.html
	 */
	cube_fast_t v3, vi, vo, vp, ret;

	v3 = _mm256_shuffle_epi8(c, c);
	v3 = _mm256_shuffle_epi8(v3, c);
	vi = _mm256_shuffle_epi8(v3, v3);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, v3);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, c);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, v3);
	vi = _mm256_shuffle_epi8(vi, vi);
	vi = _mm256_shuffle_epi8(vi, c);

	vo = _mm256_and_si256(c, _mm256_or_si256(_eo_avx2, _co2_avx2));
	vo = _mm256_shuffle_epi8(vo, vi);
	vp = _mm256_andnot_si256(_mm256_or_si256(_eo_avx2, _co2_avx2), vi);
	ret = _mm256_or_si256(vp, vo);
	ret = cleanaftershuffle(ret);
	
	return invertco_fast(ret);
}
