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
	// todo as for loop
	if (y == 0) {
		return f(xs...);
	} else {
		return g(y - 1, R<f, g>(y - 1, xs...), xs...);
	}
};


int main() {
	std::cout << U<2, 3>(1, 2, 3) << "\n";
	std::cout << S<N, N>(100) << "\n";
	std::cout << R<U<1, 1>, S<N, U<2, 3>>>(10, 5) << "\n";

	auto id = U<1, 1>;

	auto add = R<id, S<N, U<2, 3>>>;

	std::cout << add(1, 2) << "\n";

	return 0;
}
