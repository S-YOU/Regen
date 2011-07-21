#include "regex.h"
#include "time.h"
#include <re2/re2.h>

struct testcase {
  testcase(std::string regex_, std::string text_, std::string pretty_, bool result_): regex(regex_), text(text_), pretty(pretty_), result(result_) {}
  std::string regex;
  std::string text;
  std::string pretty;
  bool result;
};

struct benchresult {
  int compile_time;
  int matching_time;
  bool result;
};

int main(int argc, char *argv[]) {
  int opt;
  int thread_num = 1;
  regen::Regex::Optimize olevel = regen::Regex::Onone;

  while ((opt = getopt(argc, argv, "nf:t:O:")) != -1) {
    switch(opt) {
      case 'O': {
        olevel = regen::Regex::Optimize(atoi(optarg));
        break;
      }
      case 't': {
        thread_num = atoi(optarg);
        break;
      }
    }
  }

  std::vector<testcase> bench;
  std::string text;
  for (std::size_t i = 0; i < 100; i++) {
    for (std::size_t j = 0; j < 10; j++) {
      text += "0123456789";
    }
    text += "_";
  }
  bench.push_back(testcase("((0123456789)_?)*", text, "((0123456789){10}_){100}", true)); //RE2 #=> 3: 100 clocks for compile:matching.

  text = "a";
  for (std::size_t i = 0; i < 10; i++) {
    text += text;
  }

  bench.push_back(testcase("(a?){512}a{512}", text, "a{1024}", true));

  text += "bbbbbbbbbb";
  bench.push_back(testcase(".*b.{8}b", text, "a{1024}b{10}", true));

  /* fix..
  std::string regex = "http://((([a-zA-Z0-9]|[a-zA-Z0-9][-a-zA-Z0-9]*[a-zA-Z0-9])\\.)*([a-zA-Z]|[a-zA-Z][-a-zA-Z0-9]*[a-zA-Z0-9])\\.?|[0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)(:[0-9]*)?(/([-_.!~*'()a-zA-Z0-9:@&=+$,]|%[0-9A-Fa-f][0-9A-Fa-f])*(;([-_.!~*'()a-zA-Z0-9:@&=+$,]|%[0-9A-Fa-f][0-9A-Fa-f])*)*(/([-_.!~*'()a-zA-Z0-9:@&=+$,]|%[0-9A-Fa-f][0-9A-Fa-f])*(;([-_.!~*'()a-zA-Z0-9:@&=+$,]|%[0-9A-Fa-f][0-9A-Fa-f])*)*)*(\\?([-_.!~*'()a-zA-Z0-9;/?:@&=+$,]|%[0-9A-Fa-f][0-9A-Fa-f])*)?)?";
  text = "http://en.wikipedia.org/wiki/Parsing_expression_grammar";
  bench.push_back(testcase(regex, text, text, true));
  */
  
  clock_t start, end;
  std::vector<benchresult> result(bench.size());
  for (std::size_t i = 0; i < bench.size(); i++) {
    regen::Regex r(bench[i].regex);
    start = clock();
    r.Compile(olevel);
    end   = clock();
    result[i].compile_time = end - start;
    start = clock();
    result[i].result = r.FullMatch(bench[i].text) == bench[i].result;
    end   = clock();
    result[i].matching_time = end - start;
  }

  const char *ostr[] = {"  Onone", "     O0", "     O1", "     O2", "     O3"};
  for (std::size_t i = 0; i < bench.size(); i++) {
    printf("BENCH %"PRIuS" : regex = /%s/ text = \"%s\"\n" , i, bench[i].regex.c_str(), bench[i].pretty.c_str());
    if (!result[i].result) puts("FAIL\n");
    printf("%s : compile time = %d, matching time = %d\n", ostr[olevel+1], result[i].compile_time, result[i].matching_time);
  }
  return 0;
}