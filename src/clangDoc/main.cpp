#include <iostream>
#include "ClangWrappers.hpp"

using namespace std;
using namespace clangw;

ostream& operator<<(ostream& stream, const CXString& str);

int main() {
  shared_ptr<Index> i = make_shared<Index>(false, true);
  shared_ptr<TranslationUnit> tu = make_shared<TranslationUnit>(i->makeTranslationUnit(
    "header.hpp",
    nullptr, 0,
    nullptr, 0
  ));

  TokenArray ta{shared_ptr<TranslationUnit>(tu)};

  Cursor lastDecl{};
  unsigned int lastComment = 0;
  bool justFoundComment = false;

  for (unsigned int n = 0; n < ta.size(); ++n) {
    if (ta.kindOfTokenAt(n) == token::Kind::comment) {
      lastComment = n;
      justFoundComment = true;
      continue;
    }

    if (ta.kindOfTokenAt(n) != token::Kind::identifier)
      continue;

    Cursor c = tu->cursorAt(ta.locationOfTokenAt(n));
    if (!c.kind().declaration())
      continue;

    if (justFoundComment)
      printf("%s (%s)\n%s\n%s (%s)\n",
        lastDecl.spelling().cstr(), lastDecl.kind().spelling().cstr(),
        ta.spellingOfTokenAt(lastComment).cstr(),
        c.spelling().cstr(), c.kind().spelling().cstr());
    justFoundComment = false;

    lastDecl = c;
  }

  /*root.visitChildren([](Cursor c, Cursor, CXClientData) {
    if (!clang_Location_isFromMainFile(c.location()))
      return CXChildVisit_Continue;

    cout
      << c.spelling().cstr()
      << " (" << c.kind().spelling().cstr() << ")\n";
    return CXChildVisit_Recurse;
  });*/
}
