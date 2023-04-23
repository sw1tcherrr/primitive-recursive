#include <iostream>
#include <functional>
#include <cassert>

constexpr auto Z = [](auto) -> int {
	return 0;
};

constexpr auto N = [](int x) -> int {
	return x + 1;
};

template <size_t K, size_t N>
requires (K >= 1 && K <= N)
constexpr auto U = [](std::same_as<int> auto... xs) -> int {
	assert(sizeof...(xs) == N);
	return std::get<K - 1>(std::make_tuple(xs...));
};

template <auto g, auto... fs>
constexpr auto S = [](std::same_as<int> auto... xs) -> int {
	return g(fs(xs...)...);
};

template <auto f, auto g>
constexpr auto R = [](int y, std::same_as<int> auto... xs) -> int {
	int res = f(xs...);
	for (int yi = 0; yi < y; ++yi) {
		res = g(yi, res, xs...);
	}
	return res;
};

template <auto F>
constexpr auto flip = S<F, U<2, 2>, U<1, 2>>;

template <size_t K>
constexpr auto one = S<S<N, Z>, U<1, K>>;

template <size_t K>
constexpr auto zero = S<Z, U<1, K>>;

int main() {
	auto id = U<1, 1>;

	auto add = R<id, S<N, U<2, 3>>>;
	std::cout << add(1, 2) << "\n";

	// 1a
	auto mul = R<Z, S<add, U<2, 3>, U<3, 3>>>;
	std::cout << mul(54, 21) << "\n";

	auto dec_y = R<Z, U<1, 3>>;
	auto dec_x = S<dec_y, id, Z>;
	auto sub_ = R<id, S<dec_x, U<2, 3>>>; // x2 - x1
	auto sub = flip<sub_>; // x1 - x2
	std::cout << sub(5, 2) << "\n";

	// 1b
	auto LE = S<R<one<1>, zero<3>>, sub, U<1, 2>>;
	std::cout << LE(2, 10) << " " << LE(10, 2) << " " << LE(10, 10) << "\n";

	// 1c
	auto fact = S<R<one<1>, S<mul, S<N, U<1, 3>>, U<2, 3>>>, id, id>;
	std::cout << fact(0) << " " << fact(1) << " " << fact(5) << "\n";

	// 1d
	auto div =
			  S<
				R<zero<2>,
				  S<add, // res += 1 if y * (res+1) <= x
					U<2, 4>,
					S<LE, // y * (res+1) <= x
					  S<mul, U<4, 4>, S<N, U<2, 4>>>,
					  U<3, 4>
					>
				  >
			    >,
			    U<1, 2>, U<1, 2>, U<2, 2>
			  >;
	std::cout << div(5, 2) << " " << div(4, 2) << " " << div(100, 25) << " " << div(0, 5) << " " << div(5, 0) << "\n";
	auto mod = S<sub, U<1, 2>, S<mul, U<2, 2>, S<div, U<1, 2>, U<2, 2>>>>; // a - b * (a / b)
	std::cout << mod(5, 2) << " " << mod(4, 2) << " " << mod(121, 25) << " " << mod(0, 5) << " " << mod(5, 0) << "\n";

	return 0;
}
