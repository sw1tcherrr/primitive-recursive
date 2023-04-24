#include <iostream>
#include <functional>
#include <cassert>

// basis functions

constexpr auto Z = [](auto) -> int {
	return 0;
};

constexpr auto N = [](int x) -> int {
	return x + 1;
};

template <size_t K, size_t N>
requires (K >= 1 && K <= N)
constexpr auto U = [](std::same_as<int> auto... xs) -> int {
	if (sizeof...(xs) != N) {
		std::cerr << sizeof...(xs) << " " << N << "\n";
		assert(false);
	}
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

// helpers

constexpr auto id = U<1, 1>;

template <auto F>
constexpr auto flip = S<F, U<2, 2>, U<1, 2>>;

// numeric constants

template <int A, size_t Arity>
constexpr auto num = S<N, num<A - 1, Arity>>;

template <size_t Arity>
constexpr auto num<0, Arity> = S<Z, U<1, Arity>>;

template <size_t Arity>
constexpr auto zero = num<0, Arity>;

template <size_t Arity>
constexpr auto one = num<1, Arity>;

template <size_t Arity>
constexpr auto two = num<1, Arity>;

// projections of binary function

template <auto F, int A>
constexpr auto proj1 = S<F, num<A, 1>, id>;

template <auto F, int B>
constexpr auto proj2 = S<F, id, num<B, 1>>;

// getters in R

template <size_t N>
constexpr auto yi = U<1, N>;

template <size_t N>
constexpr auto res = U<2, N>;

template <size_t I, size_t N_xs>
constexpr auto xs = U<2 + I, 2 + N_xs>;

int main() {
	auto add = R<id, S<N, res<3>>>;
	std::cout << add(1, 2) << "\n";

	// 1a
	auto mul = R<Z, S<add, res<3>, xs<1, 1>>>;
	std::cout << mul(54, 21) << "\n";

	auto dec_y = R<Z, yi<3>>;
	auto dec_x = S<dec_y, id, Z>;
	auto sub_ = R<id, S<dec_x, res<3>>>; // x2 - x1
	auto sub = flip<sub_>; // x1 - x2
	std::cout << sub(5, 2) << "\n";

	// 1b
	auto LE = S<R<one<1>, zero<3>>, sub, U<1, 2>>;
	std::cout << LE(2, 10) << " " << LE(10, 2) << " " << LE(10, 10) << "\n";

	// 1c
	auto fact = S<R<one<1>, S<mul, S<N, yi<3>>, res<3>>>, id, id>;
	std::cout << fact(0) << " " << fact(1) << " " << fact(5) << "\n";

	// 1d
	auto div =
			  S<
				R<zero<2>,
				  S<add, // res += 1 if y * (res+1) <= x
					res<4>,
					S<LE, // y * (res+1) <= x
					  S<mul, xs<2, 2>, S<N, res<4>>>,
					  xs<1, 2>
					>
				  >
			    >,
			    U<1, 2>, U<1, 2>, U<2, 2> // x, x, y
			  >;
	std::cout << div(5, 2) << " " << div(4, 2) << " " << div(100, 25) << " " << div(0, 5) << " " << div(5, 0) << "\n";

	auto mod = S<sub, U<1, 2>, S<mul, U<2, 2>, S<div, U<1, 2>, U<2, 2>>>>; // a - b * (a / b)
	std::cout << mod(5, 2) << " " << mod(4, 2) << " " << mod(121, 25) << " " << mod(0, 5) << " " << mod(5, 0) << "\n";

	// 1e
	auto sqrt = S<
				  R<zero<1>,
				    S<add, // res += 1 if ((yi+1) * (yi+1) <= x)
					  res<3>,
					  S<LE,
					    S<mul, S<N, yi<3>>, S<N, yi<3>>>,
						xs<1, 1>
					  >
				    >
	        	  >,
				  id, id
				>;
	std::cout << sqrt(0) << " " << sqrt(1) << " " << sqrt(4) << " " << sqrt(30) << "\n";

	// 1f
	auto pair = S<R<
	                   S<div, // / 2
						 S<mul, // *
						   S<add, U<1, 2>, U<2, 2> >, // (a + b)
						   S<add, S<add, U<1, 2>, U<2, 2> >, one<2> > // (a + b + 1)
					     >,
						 two<2>
					   >,
	        		   S<N, res<4> >
				     >,
				   U<2, 2>, U<1, 2>, U<2, 2> // b, a, b
				 >;
	std::cout << pair(0, 0) << " " << pair(1, 1) << " " << pair(1, 3) << " " << pair(3, 2) << "\n";

	for (int i = 0; i < 5; ++i) {
		std::cout << proj1<pair, 1>(i) << " ";
	}
	std::cout << "\n";

	return 0;
}
