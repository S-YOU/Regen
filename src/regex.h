#ifndef REGEN_REGEX_H_
#define REGEN_REGEX_H_

#include "util.h"
#include "expr.h"
#include "exprutil.h"
#include "dfa.h"

namespace regen {

class Regex {
public:
  Regex(const std::string &regex, std::size_t recursive_depth_ = 2);
  ~Regex() { delete expr_root_; };
  void PrintRegex();
  void PrintExtendedRegex() const;
  void PrintParseTree() const;
  Expr* CreateRegexFromDFA(DFA &dfa);
  void DumpExprTree() const;
  bool Compile() { return dfa_.Compile(); }
  bool FullMatch(const std::string &string) const;
  bool FullMatch(const unsigned char *begin, const unsigned char *end) const;
  const std::string& regex() const { return regex_; }
  const Must& must() const { return must_; }
  std::size_t max_lenlgth() const { return expr_root_->max_length(); }
  std::size_t min_lenlgth() const { return expr_root_->min_length(); }
  std::size_t must_max_length() const { return must_max_length_; }
  const std::string& must_max_word() const { return must_max_word_; }
  const DFA& dfa() const { return dfa_; }

private:
  Expr::Type lex();
  Expr* e0();
  Expr* e1();
  Expr* e2();
  Expr* e3();
  Expr* e4();
  void Parse();
  void MakeDFA(Expr* e, DFA &dfa, std::size_t neop = 1);
  CharClass* BuildCharClass();
  StateExpr* CombineStateExpr(StateExpr* e1, StateExpr* e2);

  const std::string regex_;
  Expr *expr_root_;
  std::stack<const char *> parse_stack_;
  std::size_t recursive_depth_;
  std::size_t recursive_limit_;

  Must must_;
  std::size_t must_max_length_;
  const std::string must_max_word_;
  std::bitset<256> involved_char_;
  std::size_t count_involved_char_;

  Expr::Type token_type_;
  const char *parse_ptr_;
  char parse_lit_;
  int lower_repetition_, upper_repetition_;
  std::size_t expr_id_;
  std::size_t state_id_;

  DFA dfa_;
};

} // namespace regen

#endif // REGEN_REGEX_H_
