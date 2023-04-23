#include <iostream>
#include <array>
#include <functional>


template <size_t N>
using vec = std::array<int, N>;

template <size_t N, typename F>
concept fun = requires(F&& f, vec<N> const& x) {
	{ std::invoke(std::forward<F>(f), x) } -> std::same_as<int>;
};

constexpr int Z(auto) {
	return 0;
}

constexpr int N(vec<1> x) {
	return x[0] + 1;
}

template <size_t K, size_t N>
constexpr int U(vec<N> const& x) {
	return x[K];
}

template <size_t K, size_t N, auto g, auto... fs>
requires fun<K, decltype(g)> && (fun<N, decltype(fs)> &&...)
constexpr int S(vec<N> const& x) {
	return g({fs(x)...});
}


template <size_t N, auto f, auto g>
requires fun<N + 2, decltype(g)> && fun<N, decltype(f)>
constexpr int R(vec<N> const& x, int y) {
	if (y == 0) {
		return f(x);
	} else {
		vec<N + 2> arg;
		for (int i = 0; i < N; ++i) {
			arg[i] = x[i];
		}
		arg[N] = y - 1;
		arg[N + 1] = R<N, f, g>(x, y - 1);
		return g(arg);
	}
}


int main() {
	std::cout << U<1, 3>({1, 2, 3}) << "\n";
	std::cout << S<1, 1, N, N>({100}) << "\n";
	std::cout << R<1, U<0, 1>, S<1, 3, N, U<2, 3>>>({5}, 10) << "\n";

	auto id = U<0, 1>;

	int (*add)(vec<1> const&, int) = &R<1,
				id,
				S<1, 3, N, U<2, 3>>>;

//	auto add_self = S<1, 1, add, id>;
//
//	auto mul = R<1,
//	        	Z,
//				add_self>;

	std::cout << mul({5}, 3);

	return 0;
}
