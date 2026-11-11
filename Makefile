.PHONY: format

format:
	find . \( -path './.git' -o -path './build' -o -path './data' \) -prune -o \
		\( -name '*.cpp' -o -name '*.hpp' \) -print0 | \
		xargs -0 xcrun clang-format -i
