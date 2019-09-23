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

  Cursor root = tu->getRootCursor();

  {
    TokenArray ta{shared_ptr<TranslationUnit>(tu)};

    for (unsigned int n = 0; n < ta.size(); ++n)
      printf("%s\n", ta.spellingAt(n).cstr());
  }

  root.visitChildren([](Cursor c, Cursor, CXClientData) {
    if (!clang_Location_isFromMainFile(c.location()))
      return CXChildVisit_Continue;

    cout
      << c.spelling().cstr()
      << " (" << c.kind().spelling().cstr() << ")\n";
    return CXChildVisit_Recurse;
  });
}
