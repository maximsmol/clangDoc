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

  bool lookingForDecl = false;
  unsigned int commentAt = 0;

  for (unsigned int n = 0; n < ta.size(); ++n) {
    if (ta.kindOfTokenAt(n) == token::Kind::Comment) {
      lookingForDecl = true;
      commentAt = n;
      continue;
    }

    if (!lookingForDecl)
      continue;

    Cursor c = tu->cursorAt(ta.locationOfTokenAt(n));
    if (!c.kind().declaration())
      continue;

    lookingForDecl = false;

    printf("%s\n%s (%s)\n", ta.spellingOfTokenAt(commentAt).cstr(), c.spelling().cstr(), c.kind().spelling().cstr());
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
