#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int match(char *regexp, char *text);
int matchhere(char *regexp, char *text);
int matchdelimiter(char c, char delimiter, char *regexp, char *text);
int matchstar(char c, char *regexp, char *text);
int matchplus(char c, char *regexp, char *text);
int matchquestionmark(char c, char *regexp, char *text);

void test(char *regexp, char *text, bool expected);

// 在text中查找regexp
int match(char *regexp, char *text) {
	if (regexp[0] == '^') {
		return matchhere(regexp+1, text);
	}
	do {
		if (matchhere(regexp, text)) {
			return 1;
		}
	} while (*text++ != '\0');
	return 0;
}

// 在text的开头查找regexp
int matchhere(char *regexp, char *text) {
	if (regexp[0] == '\0') {
		return 1;
	}
	if (regexp[0] == '$' && regexp[1] == '\0') {
		return *text == '\0';
	}
	if (matchdelimiter(regexp[0], regexp[1], regexp+2, text)) {
		return 1;
	}

	if (*text != '\0') {
		if (regexp[0] == '[') {
			char *p = regexp+1, *q;
			bool flag = false;
			while (*p != '\0') {
				if (*p == ']') {
					flag = true;
					break;
				}
				++p;
			}
			if (flag) {
				for (q = regexp+1; q < p; ++q) {
					if (*q == '.' || *q == *text) {	
						if (matchhere(p+1, text+1)) {
							return 1;
						}
					}
					if (matchdelimiter(*q, p[1], p+2, text)) {
						return 1;
					}
				}
			}
		}
		if (regexp[0] == '.' || regexp[0] == *text) {
			return matchhere(regexp+1, text+1);
		}
	}
	return 0;
}

// 在text的开头匹配含有限制符(*, +, ?)的regexp
int matchdelimiter(char c, char delimiter, char *regexp, char *text) {
	// printf("matchdelimiter:[%c]-[%c]-[%s]-[%s]\n", c, delimiter, regexp, text);
	if (delimiter == '*') {
		return matchstar(c, regexp, text);
	}
	if (delimiter == '+') {
		return matchplus(c, regexp, text);
	}
	if (delimiter == '?') {
		return matchquestionmark(c, regexp, text);
	}
	return 0;
}

// 在text的开头查找c*regexp
int matchstar(char c, char *regexp, char *text) {
	do {
		if (matchhere(regexp, text)) {
			return 1;
		}
	} while (*text != '\0' && (*text++ == c || c == '.'));
	return 0;
}

// 在text的开头查找c+regexp
int matchplus(char c, char *regexp, char *text) {
	while (*text != '\0' && (*text++ == c || c == '.')) {
		if (matchhere(regexp, text)) {
			return 1;
		}
	}
	return 0;
}

// 在text的开头查找c?regexp
int matchquestionmark(char c, char *regexp, char *text) {
	if (*text != '\0' && (*text == c || c == '.')) {
		if (matchhere(regexp, text+1)) {
			return 1;
		}
	}
	return matchhere(regexp, text);
}

// 测试
void test(char *regexp, char *text, bool expected) {
	if (expected && !match(regexp, text)) {
		printf("[expect match]: but [%s] not match [%s]\n", regexp, text);
		exit(-1);
	}
	if (!expected && match(regexp, text)) {
		printf("[expect not match]: but [%s] match [%s]\n", regexp, text);
		exit(-1);
	}
}

int main() {
	test("a", "", false);
	test("a*", "", true);
	test("a*", "b", true);
	test("a*b", "aaabb", true);
	test("a+", "a", true);
	test("a+", "", false);
	test("a+", "b", false);
	test("a+b", "aaabb", true);
	test("a+b", "bb", false);
	test("a?", "b", true);
	test("a?b", "b", true);
	test("a?b", "ab", true);
	test("aa?bb", "abb", true);
	test(".*d", "helloworld", true);
	test("hel+o.*world$", "helloworld", true);
	test("^hel*.*wo.*d$", "helloworld", true);
	test("a*bb*", "aaaabbbb", true);
	test("^abb", "aabb", false);
	test("[abc]", "", false);
	test("[abc]", "a", true);
	test("[abc]", "b", true);
	test("[abc]", "c", true);
	test("[abc]d", "ad", true);
	test("[abc]d", "bd", true);
	test("[abc]d", "cd", true);
	test("[abc]d", "dd", false);
	test("[abc]+", "a", true);
	test("[abc]*", "b", true);
	test("[abc]*", "cc", true);
	test("[abc]*", "d", true);
	test("[ab]jj[xyz]+[def]*", "bjjyy", true);
	test("^hello.*X+, love [abc][def]?[op]+", "hello, my name is XXX, love cpp", true);

	puts("test success");
	
	return 0;
}

